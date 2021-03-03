/**
 * @file TimingHardwareManager.cpp TimingHardwareManager class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingHardwareManagerPDI.hpp"

#include "timing/timinghardwaremanagerpdi/Structs.hpp"
#include "timing/timinghardwaremanagerpdi/Nljs.hpp"

#include "timing/timingcmd/Structs.hpp"
#include "timing/timingcmd/Nljs.hpp"

#include "TimingIssues.hpp"

#include "appfwk/DAQModuleHelper.hpp"
#include "appfwk/cmd/Nljs.hpp"

#include "ers/Issue.hpp"
#include "logging/Logging.hpp"

#include <chrono>
#include <cstdlib>
#include <thread>
#include <string>
#include <vector>
#include <memory>

namespace dunedaq {
namespace timing {

TimingHardwareManagerPDI::TimingHardwareManagerPDI(const std::string& name)
  : TimingHardwareManager<pdt::PDIMasterDesign<pdt::TLUIONode>,pdt::EndpointDesign<pdt::FMCIONode>>(name)
  
  // default gather interval of 1e6 us, may be overidden by config below
  , m_master_monitor_data_gatherer ( std::bind(&TimingHardwareManagerPDI::gather_master_monitor_data, this, std::placeholders::_1), 1e6 )
  , m_endpoint_monitor_data_gatherer ( std::bind(&TimingHardwareManagerPDI::gather_endpoint_monitor_data, this, std::placeholders::_1), 1e6 )
  
  // default gather interval of 10e6 us, may be overidden by config below
  , m_master_monitor_data_gatherer_debug ( std::bind(&TimingHardwareManagerPDI::gather_master_monitor_data_debug, this, std::placeholders::_1), 10e6 )
  , m_endpoint_monitor_data_gatherer_debug ( std::bind(&TimingHardwareManagerPDI::gather_endpoint_monitor_data_debug, this, std::placeholders::_1), 10e6 )

{ 
  register_command("conf", &TimingHardwareManagerPDI::do_configure);

  // register only the commands which are needed for this hardware manager
  
  // commands for master device
  register_timing_hw_command("master_io_reset", &TimingHardwareManagerPDI::master_io_reset);
  register_timing_hw_command("master_set_timestamp", &TimingHardwareManagerPDI::master_set_timestamp);
  register_timing_hw_command("master_print_status", &TimingHardwareManagerPDI::master_print_status);

  // commands for timing partitions
  register_timing_hw_command("partition_configure", &TimingHardwareManagerPDI::partition_configure);
  register_timing_hw_command("partition_enable", &TimingHardwareManagerPDI::partition_enable);
  register_timing_hw_command("partition_disable", &TimingHardwareManagerPDI::partition_disable);
  register_timing_hw_command("partition_start", &TimingHardwareManagerPDI::partition_start);
  register_timing_hw_command("partition_stop", &TimingHardwareManagerPDI::partition_stop);
  register_timing_hw_command("partition_enable_triggers", &TimingHardwareManagerPDI::partition_enable_triggers);
  register_timing_hw_command("partition_disable_triggers", &TimingHardwareManagerPDI::partition_disable_triggers);
  register_timing_hw_command("partition_print_status", &TimingHardwareManagerPDI::partition_print_status);
  
  // commands endpoint device
  register_timing_hw_command("endpoint_io_reset", &TimingHardwareManagerPDI::endpoint_io_reset);
  register_timing_hw_command("endpoint_enable", &TimingHardwareManagerPDI::endpoint_enable);
  register_timing_hw_command("endpoint_disable", &TimingHardwareManagerPDI::endpoint_disable);
  register_timing_hw_command("endpoint_reset", &TimingHardwareManagerPDI::endpoint_reset);
  register_timing_hw_command("endpoint_print_status", &TimingHardwareManagerPDI::endpoint_print_status);
}

void
TimingHardwareManagerPDI::do_configure(const nlohmann::json& obj)
{
  timinghardwaremanagerpdi::from_json(obj, m_cfg);
  
  m_connections_file = m_cfg.connections_file;

  m_master_monitor_data_gatherer.update_gather_interval(m_cfg.gather_interval);
  m_endpoint_monitor_data_gatherer.update_gather_interval(m_cfg.gather_interval);

  // TODO debug gather interval config

  TLOG() << get_name() << "conf: con. file before env var expansion: " << m_connections_file;
  resolve_environment_variables(m_connections_file);
  TLOG() << get_name() << "conf: con. file after env var expansion:  " << m_connections_file;

  // uhal log level to be passed in as parameter?
  //uhal::setLogLevelTo(uhal::Notice()); 

  try
  {
    m_connection_manager = std::make_unique< uhal::ConnectionManager >("file://"+m_connections_file);
  }
  catch (const uhal::exception::FileNotFound& excpt)
  {
    std::stringstream message;
    message << m_connections_file << " not found. Have you set PDT_TESTS?";
    throw UHALConnectionsFileIssue(ERS_HERE, message.str(), excpt);
  }
}

void
TimingHardwareManagerPDI::do_start(const nlohmann::json&)
{ 
  // only start monitor threads if we have been given the name of the device to monitor
  if (m_cfg.monitored_device_name_master.compare(""))
  {
    m_master_monitor_data_gatherer.start_gathering_thread();
    m_master_monitor_data_gatherer_debug.start_gathering_thread();
  } 
  if (m_cfg.monitored_device_name_endpoint.compare(""))
  {
    m_endpoint_monitor_data_gatherer.start_gathering_thread();
    m_endpoint_monitor_data_gatherer_debug.start_gathering_thread();
  } 

  thread_.start_working_thread();
  TLOG() << get_name() << " successfully started";
}

void
TimingHardwareManagerPDI::do_stop(const nlohmann::json&)
{
  // do not attempt to stop monitor threads if we have not been given the name of the device to monitor
  if (m_cfg.monitored_device_name_master.compare(""))
  {
   m_master_monitor_data_gatherer.stop_gathering_thread();
   m_master_monitor_data_gatherer_debug.stop_gathering_thread();
  } 
  if (m_cfg.monitored_device_name_endpoint.compare(""))
  {
    m_endpoint_monitor_data_gatherer.stop_gathering_thread();
    m_endpoint_monitor_data_gatherer_debug.stop_gathering_thread();
  }

  thread_.stop_working_thread();
  TLOG() << get_name() << " successfully stopped";
}

void
TimingHardwareManagerPDI::gather_master_monitor_data(InfoGatherer<pdt::timingmon::TimingPDIMasterTLUMonitorData>& gatherer)
{
  while (gatherer.run_gathering())
  {
    // monitoring data recepticle
    pdt::timingmon::TimingPDIMasterTLUMonitorData mon_data;

    // collect the data from the hardware
    auto master_design = get_timing_device<pdt::PDIMasterDesign<pdt::TLUIONode>>(m_cfg.monitored_device_name_master);    
    master_design.get_io_node().get_info(mon_data.hardware_data);
    master_design.get_master_node().get_info(mon_data.firmware_data);

    // when do we actually collect the data
    mon_data.time_gathered = static_cast<int64_t>(std::time(nullptr));

    // store the monitor data for retrieveal by get_info at a later time
    gatherer.update_monitoring_data(mon_data);

    // sleep for a bit
    usleep(gatherer.get_gather_interval());
  }
}

void
TimingHardwareManagerPDI::gather_endpoint_monitor_data(InfoGatherer<pdt::timingmon::TimingEndpointFMCMonitorData>& gatherer)
{
  while (gatherer.run_gathering())
  {
    // monitoring data recepticle
    pdt::timingmon::TimingEndpointFMCMonitorData mon_data;

    // collect the data from the hardware
    auto endpoint_design = get_timing_device<pdt::EndpointDesign<pdt::FMCIONode>>(m_cfg.monitored_device_name_endpoint);    
    endpoint_design.get_io_node().get_info(mon_data.hardware_data);
    endpoint_design.get_endpoint_node(0).get_info(mon_data.firmware_data);

    // when do we actually collect the data
    mon_data.time_gathered = static_cast<int64_t>(std::time(nullptr));

    // store the monitor data for retrieveal by get_info at a later time
    gatherer.update_monitoring_data(mon_data);

    // sleep for a bit
    usleep(gatherer.get_gather_interval());
  }
}

void
TimingHardwareManagerPDI::gather_master_monitor_data_debug(InfoGatherer<pdt::timingmon::TimingPDIMasterTLUMonitorDataDebug>& gatherer)
{
  while (gatherer.run_gathering())
  {
    // monitoring data recepticle
    pdt::timingmon::TimingPDIMasterTLUMonitorDataDebug mon_data;

    // collect the data from the hardware
    auto master_design = get_timing_device<pdt::PDIMasterDesign<pdt::TLUIONode>>(m_cfg.monitored_device_name_master);    
    master_design.get_io_node().get_info(mon_data.hardware_data);
    master_design.get_master_node().get_info(mon_data.firmware_data);

    // when do we actually collect the data
    mon_data.time_gathered = static_cast<int64_t>(std::time(nullptr));

    // store the monitor data for retrieveal by get_info at a later time
    gatherer.update_monitoring_data(mon_data);

    // sleep for a bit
    usleep(gatherer.get_gather_interval());
  }
}

void
TimingHardwareManagerPDI::gather_endpoint_monitor_data_debug(InfoGatherer<pdt::timingmon::TimingEndpointFMCMonitorDataDebug>& gatherer)
{
  while (gatherer.run_gathering())
  {
    // monitoring data recepticle
    pdt::timingmon::TimingEndpointFMCMonitorDataDebug mon_data;

    // collect the data from the hardware
    auto endpoint_design = get_timing_device<pdt::EndpointDesign<pdt::FMCIONode>>(m_cfg.monitored_device_name_endpoint);    
    endpoint_design.get_io_node().get_info(mon_data.hardware_data);
    endpoint_design.get_endpoint_node(0).get_info(mon_data.firmware_data);

    // when do we actually collect the data
    mon_data.time_gathered = static_cast<int64_t>(std::time(nullptr));

    // store the monitor data for retrieveal by get_info at a later time
    gatherer.update_monitoring_data(mon_data);

    // sleep for a bit
    usleep(gatherer.get_gather_interval());
  }
}

void
TimingHardwareManagerPDI::get_info(opmonlib::InfoCollector & ci, int level)
{
  auto master_mon_data = m_master_monitor_data_gatherer.get_monitoring_data();
  auto endpoint_mon_data = m_endpoint_monitor_data_gatherer.get_monitoring_data();

  auto master_mon_data_debug = m_master_monitor_data_gatherer_debug.get_monitoring_data();
  auto endpoint_mon_data_debug = m_endpoint_monitor_data_gatherer_debug.get_monitoring_data();

  // maybe we should keep track of when we last send data, and only send if we have had an update since
  // only send data if we it has been gathered at least once
  if (master_mon_data.time_gathered != 0) ci.add(master_mon_data);
  if (endpoint_mon_data.time_gathered != 0) ci.add(endpoint_mon_data);

  if (level > 0) {
    if (master_mon_data_debug.time_gathered != 0) ci.add(master_mon_data_debug);
    if (endpoint_mon_data_debug.time_gathered != 0) ci.add(endpoint_mon_data_debug);
  }
}
} // namespace timing 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timing::TimingHardwareManagerPDI)

// Local Variables:
// c-basic-offset: 2
// End:

/**
 * @file TimingHardwareManager.cpp TimingHardwareManager class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingHardwareManagerPDI.hpp"

#include "timinglibs/timinghardwaremanagerpdi/Structs.hpp"
#include "timinglibs/timinghardwaremanagerpdi/Nljs.hpp"

#include "timinglibs/timingcmd/Structs.hpp"
#include "timinglibs/timingcmd/Nljs.hpp"

#include "timinglibs/TimingIssues.hpp"

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
namespace timinglibs {

TimingHardwareManagerPDI::TimingHardwareManagerPDI(const std::string& name)
  : TimingHardwareManager<timing::PDIMasterDesign<timing::TLUIONode>,timing::EndpointDesign<timing::FMCIONode>>(name)
  
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

  m_master_monitor_data_gatherer_debug.update_gather_interval(m_cfg.gather_interval_debug);
  m_endpoint_monitor_data_gatherer_debug.update_gather_interval(m_cfg.gather_interval_debug);

  TLOG() << get_name() << "conf: con. file before env var expansion: " << m_connections_file;
  resolve_environment_variables(m_connections_file);
  TLOG() << get_name() << "conf: con. file after env var expansion:  " << m_connections_file;

  if (!m_cfg.uhal_log_level.compare("debug"))
  {
    uhal::setLogLevelTo ( uhal::Debug() );
  }
  else if (!m_cfg.uhal_log_level.compare("info"))
  {
    uhal::setLogLevelTo ( uhal::Info() );
  }
  else if (!m_cfg.uhal_log_level.compare("notice"))
  {
    uhal::setLogLevelTo ( uhal::Notice() );
  }
  else if (!m_cfg.uhal_log_level.compare("warning"))
  {
    uhal::setLogLevelTo ( uhal::Warning() );
  }
  else if (!m_cfg.uhal_log_level.compare("error"))
  {
    uhal::setLogLevelTo ( uhal::Error() );
  }  
  else if (!m_cfg.uhal_log_level.compare("fatal"))
  {
    uhal::setLogLevelTo ( uhal::Fatal() );
  }
  else
  {
    throw InvalidUHALLogLevel(ERS_HERE, m_cfg.uhal_log_level);
  }

  try
  {
    m_connection_manager = std::make_unique< uhal::ConnectionManager >("file://"+m_connections_file);
  }
  catch (const uhal::exception::FileNotFound& excpt)
  {
    std::stringstream message;
    message << m_connections_file << " not found. Has TIMING_SHARE been set?";
    throw UHALConnectionsFileIssue(ERS_HERE, message.str(), excpt);
  }
}

void
TimingHardwareManagerPDI::do_start(const nlohmann::json&)
{ 
  m_received_hw_commands_counter = 0;
  m_accepted_hw_commands_counter = 0;
  m_rejected_hw_commands_counter = 0;
  
  start_hw_mon_gathering();

  thread_.start_working_thread();
  TLOG() << get_name() << " successfully started";
}

void
TimingHardwareManagerPDI::do_stop(const nlohmann::json&)
{
  stop_hw_mon_gathering();
  thread_.stop_working_thread();
  TLOG() << get_name() << " successfully stopped";
}

void
TimingHardwareManagerPDI::start_hw_mon_gathering()
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
}

void
TimingHardwareManagerPDI::stop_hw_mon_gathering()
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
}

void
TimingHardwareManagerPDI::gather_master_monitor_data(InfoGatherer<timing::timingfirmwareinfo::TimingPDIMasterTLUMonitorData>& gatherer)
{
  while (gatherer.run_gathering())
  {
    // monitoring data recepticle
    timing::timingfirmwareinfo::TimingPDIMasterTLUMonitorData mon_data;

    int successful_infos_gathered = 2;

    // collect the data from the hardware
    try
    {
      auto master_design = get_timing_device<timing::PDIMasterDesign<timing::TLUIONode>>(m_cfg.monitored_device_name_master); 
      master_design.get_io_node().get_info(mon_data.hardware_data);
    }
    catch (const std::exception& excpt)
    {
      --successful_infos_gathered;
      ers::error(FailedToCollectOpMonInfo(ERS_HERE, mon_data.hardware_data.class_name, m_cfg.monitored_device_name_master, excpt));
    }
    
    try
    {
      auto master_design = get_timing_device<timing::PDIMasterDesign<timing::TLUIONode>>(m_cfg.monitored_device_name_master); 
      master_design.get_master_node().get_info(mon_data.firmware_data);  
    }
    catch (const std::exception& excpt)
    {
      --successful_infos_gathered;
      ers::error(FailedToCollectOpMonInfo(ERS_HERE, mon_data.firmware_data.class_name, m_cfg.monitored_device_name_master, excpt));
    }

    // did we actually manage to gather any new data?
    if (successful_infos_gathered > 0)
    {
      // when did we actually collect the data
      mon_data.time_gathered = static_cast<int64_t>(std::time(nullptr));

      // store the monitor data for retrieveal by get_info at a later time
      gatherer.update_monitoring_data(mon_data);
    } 
    
    // sleep for a bit
    usleep(gatherer.get_gather_interval());
  }
}

void
TimingHardwareManagerPDI::gather_endpoint_monitor_data(InfoGatherer<timing::timingfirmwareinfo::TimingEndpointFMCMonitorData>& gatherer)
{
  while (gatherer.run_gathering())
  {
    // monitoring data recepticle
    timing::timingfirmwareinfo::TimingEndpointFMCMonitorData mon_data;
    
    int successful_infos_gathered = 2;

    // collect the data from the hardware    
    try
    {
      auto endpoint_design = get_timing_device<timing::EndpointDesign<timing::FMCIONode>>(m_cfg.monitored_device_name_endpoint);
      endpoint_design.get_io_node().get_info(mon_data.hardware_data);  
    }
    catch(const std::exception& excpt)
    {
      ers::error(FailedToCollectOpMonInfo(ERS_HERE, mon_data.hardware_data.class_name, m_cfg.monitored_device_name_endpoint, excpt));
      --successful_infos_gathered;
    }

    try
    {
      auto endpoint_design = get_timing_device<timing::EndpointDesign<timing::FMCIONode>>(m_cfg.monitored_device_name_endpoint);
      endpoint_design.get_endpoint_node(0).get_info(mon_data.firmware_data);  
    }
    catch(const std::exception& excpt)
    {
      ers::error(FailedToCollectOpMonInfo(ERS_HERE, mon_data.firmware_data.class_name, m_cfg.monitored_device_name_endpoint, excpt));
      --successful_infos_gathered;
    }

    // did we actually manage to gather any new data?
    if (successful_infos_gathered > 0)
    {
      // when did we actually collect the data
      mon_data.time_gathered = static_cast<int64_t>(std::time(nullptr));

      // store the monitor data for retrieveal by get_info at a later time
      gatherer.update_monitoring_data(mon_data);
    }

    // sleep for a bit
    usleep(gatherer.get_gather_interval());
  }
}

void
TimingHardwareManagerPDI::gather_master_monitor_data_debug(InfoGatherer<timing::timingfirmwareinfo::TimingPDIMasterTLUMonitorDataDebug>& gatherer)
{
  while (gatherer.run_gathering())
  {
    // monitoring data recepticle
    timing::timingfirmwareinfo::TimingPDIMasterTLUMonitorDataDebug mon_data;

    int successful_infos_gathered = 1;

    // collect the data from the hardware
    try
    {
      auto master_design = get_timing_device<timing::PDIMasterDesign<timing::TLUIONode>>(m_cfg.monitored_device_name_master); 
      master_design.get_io_node().get_info(mon_data.hardware_data);  
    }
    catch(const std::exception& excpt)
    {
      ers::error(FailedToCollectOpMonInfo(ERS_HERE, mon_data.hardware_data.class_name, m_cfg.monitored_device_name_master, excpt));
      --successful_infos_gathered;
    }

    // did we actually manage to gather any new data?
    if (successful_infos_gathered > 0) {
      // when did we actually collect the data
      mon_data.time_gathered = static_cast<int64_t>(std::time(nullptr));

      // store the monitor data for retrieveal by get_info at a later time
      gatherer.update_monitoring_data(mon_data);
    }
    

    // sleep for a bit
    usleep(gatherer.get_gather_interval());
  }
}

void
TimingHardwareManagerPDI::gather_endpoint_monitor_data_debug(InfoGatherer<timing::timingfirmwareinfo::TimingEndpointFMCMonitorDataDebug>& gatherer)
{
  while (gatherer.run_gathering())
  {
    // monitoring data recepticle
    timing::timingfirmwareinfo::TimingEndpointFMCMonitorDataDebug mon_data;

    int successful_infos_gathered = 1;

    // collect the data from the hardware   
    try
    {
      auto endpoint_design = get_timing_device<timing::EndpointDesign<timing::FMCIONode>>(m_cfg.monitored_device_name_endpoint); 
      endpoint_design.get_io_node().get_info(mon_data.hardware_data);
    }
    catch(const std::exception& excpt)
    {
      ers::error(FailedToCollectOpMonInfo(ERS_HERE, mon_data.hardware_data.class_name, m_cfg.monitored_device_name_endpoint, excpt));
      --successful_infos_gathered;
    }

    // did we actually manage to gather any new data?
    if (successful_infos_gathered > 0) 
    { 
      // when did we actually gather the data
      mon_data.time_gathered = static_cast<int64_t>(std::time(nullptr));
      
      // store the monitor data for retrieveal by get_info at a later time
      gatherer.update_monitoring_data(mon_data);
    }
    

    // sleep for a bit
    usleep(gatherer.get_gather_interval());
  }
}

void
TimingHardwareManagerPDI::get_info(opmonlib::InfoCollector & ci, int level)
{

  // send counters internal to the module
  timinghardwaremanagerpdiinfo::Info module_info;
  module_info.received_hw_commands_counter = m_received_hw_commands_counter.load();
  module_info.accepted_hw_commands_counter = m_accepted_hw_commands_counter.load();
  module_info.rejected_hw_commands_counter = m_rejected_hw_commands_counter.load();
  ci.add(module_info);

  // retrieve and send hardware info
  auto master_mon_data = m_master_monitor_data_gatherer.get_monitoring_data();
  auto endpoint_mon_data = m_endpoint_monitor_data_gatherer.get_monitoring_data();

  auto master_mon_data_debug = m_master_monitor_data_gatherer_debug.get_monitoring_data();
  auto endpoint_mon_data_debug = m_endpoint_monitor_data_gatherer_debug.get_monitoring_data();
  
  // only send data if we it has been gathered at least once
  if (master_mon_data.time_gathered != 0) ci.add(master_mon_data);
  if (endpoint_mon_data.time_gathered != 0) ci.add(endpoint_mon_data);

  if (level > 1) {
    if (master_mon_data_debug.time_gathered != 0) ci.add(master_mon_data_debug);
    if (endpoint_mon_data_debug.time_gathered != 0) ci.add(endpoint_mon_data_debug);
  }

  // maybe we should keep track of when we last send data, and only send if we have had an update since

}
} // namespace timinglibs 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timinglibs::TimingHardwareManagerPDI)

// Local Variables:
// c-basic-offset: 2
// End:

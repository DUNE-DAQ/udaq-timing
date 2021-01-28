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

#include "CommonIssues.hpp"

#include "appfwk/DAQModuleHelper.hpp"
#include "appfwk/cmd/Nljs.hpp"

#include "ers/ers.h"

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
  
  m_connections_file = m_cfg.connectionsFile;
  
  ERS_INFO( get_name() << "conf: con. file before env var expansion: " << m_connections_file);
  resolve_environment_variables(m_connections_file);
  ERS_INFO( get_name() << "conf: con. file after env var expansion:  " << m_connections_file);

  // uhal log level to be passed in as parameter?
  //uhal::setLogLevelTo(uhal::Notice());  
  m_connection_manager = std::make_unique< uhal::ConnectionManager >("file://"+m_connections_file);
}

} // namespace timing 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timing::TimingHardwareManagerPDI)

// Local Variables:
// c-basic-offset: 2
// End:

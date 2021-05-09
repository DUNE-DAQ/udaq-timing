/**
 * @file HSIController.cpp HSIController class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "HSIController.hpp"

#include "timinglibs/hsicontroller/Structs.hpp"
#include "timinglibs/hsicontroller/Nljs.hpp"

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

namespace dunedaq {
namespace timinglibs {

HSIController::HSIController(const std::string& name)
  : dunedaq::timinglibs::TimingController(name, 9)   // 2nd arg: how many hw commands can this module send?
{
  register_command("conf", &HSIController::do_configure);
  register_command("start", &HSIController::do_start);
  register_command("stop",  &HSIController::do_stop);
  
  // timing endpoint hardware commands
  register_command("hsi_io_reset", &HSIController::do_hsi_io_reset);
  register_command("hsi_endpoint_enable", &HSIController::do_hsi_endpoint_enable);
  register_command("hsi_endpoint_disable", &HSIController::do_hsi_endpoint_disable);
  register_command("hsi_endpoint_reset", &HSIController::do_hsi_endpoint_reset);
  register_command("hsi_reset", &HSIController::do_hsi_reset);
  register_command("hsi_configure", &HSIController::do_hsi_configure);
  register_command("hsi_start", &HSIController::do_hsi_start);
  register_command("hsi_stop", &HSIController::do_hsi_stop);
  register_command("hsi_print_status", &HSIController::do_hsi_print_status);
}

void
HSIController::do_configure(const nlohmann::json& obj)
{
  hsicontroller::from_json(obj,m_cfg);
  
  TLOG_DEBUG(0) << get_name() << " conf: managed hsi, device: " << m_cfg.device;
}

void
HSIController::construct_hsi_hw_cmd(timingcmd::TimingHwCmd& hw_cmd, const std::string& cmd_id) {
  hw_cmd.id = cmd_id;
  hw_cmd.device = m_cfg.device;
}

void
HSIController::do_hsi_io_reset(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_hsi_hw_cmd(hw_cmd, "io_reset");
  send_hw_cmd(hw_cmd);
  ++m_sent_hw_command_counters.at(0).atomic;
}

void
HSIController::do_hsi_endpoint_enable(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_hsi_hw_cmd(hw_cmd, "endpoint_enable");
  send_hw_cmd(hw_cmd);
  ++m_sent_hw_command_counters.at(1).atomic;
}

void
HSIController::do_hsi_endpoint_disable(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_hsi_hw_cmd(hw_cmd, "endpoint_disable");
  send_hw_cmd(hw_cmd);
  ++m_sent_hw_command_counters.at(2).atomic;
}

void
HSIController::do_hsi_endpoint_reset(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_hsi_hw_cmd(hw_cmd, "endpoint_reset");
  send_hw_cmd(hw_cmd);
  ++m_sent_hw_command_counters.at(3).atomic;
}

void
HSIController::do_hsi_reset(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_hsi_hw_cmd(hw_cmd, "hsi_reset");
  send_hw_cmd(hw_cmd);
  ++m_sent_hw_command_counters.at(4).atomic;
}

void
HSIController::do_hsi_configure(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_hsi_hw_cmd(hw_cmd, "hsi_configure");
  send_hw_cmd(hw_cmd);
  ++m_sent_hw_command_counters.at(5).atomic;
}

void
HSIController::do_hsi_start(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_hsi_hw_cmd(hw_cmd, "hsi_start");
  send_hw_cmd(hw_cmd);
  ++m_sent_hw_command_counters.at(6).atomic;
}

void
HSIController::do_hsi_stop(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_hsi_hw_cmd(hw_cmd, "hsi_stop");
  send_hw_cmd(hw_cmd);
  ++m_sent_hw_command_counters.at(7).atomic;
}

void
HSIController::do_hsi_print_status(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_hsi_hw_cmd(hw_cmd, "print_status");
  send_hw_cmd(hw_cmd);
  ++m_sent_hw_command_counters.at(8).atomic;
}

void
HSIController::get_info(opmonlib::InfoCollector & ci, int /*level*/)
{
  // send counters internal to the module
  hsicontrollerinfo::Info module_info;
  for (uint i=0; i < m_number_hw_commands; ++i) {
    module_info.sent_hw_command_counters.push_back( m_sent_hw_command_counters.at(i).atomic.load() );
  }
  ci.add(module_info);
}
} // namespace timinglibs 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timinglibs::HSIController)

// Local Variables:
// c-basic-offset: 2
// End:

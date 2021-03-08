/**
 * @file TimingEndpointController.cpp TimingEndpointController class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingEndpointController.hpp"

#include "timinglibs/timingendpointcontroller/Structs.hpp"
#include "timinglibs/timingendpointcontroller/Nljs.hpp"

#include "timinglibs/timingcmd/Structs.hpp"
#include "timinglibs/timingcmd/Nljs.hpp"

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

namespace dunedaq {
namespace timinglibs {

TimingEndpointController::TimingEndpointController(const std::string& name)
  : dunedaq::timinglibs::TimingController(name)
{
  register_command("conf", &TimingEndpointController::do_configure);
  register_command("start", &TimingEndpointController::do_start);
  register_command("stop",  &TimingEndpointController::do_stop);
  
  // timing commands
  register_command("endpoint_io_reset", &TimingEndpointController::do_endpoint_io_reset);
  register_command("endpoint_enable", &TimingEndpointController::do_endpoint_enable);
  register_command("endpoint_disable", &TimingEndpointController::do_endpoint_disable);
  register_command("endpoint_reset", &TimingEndpointController::do_endpoint_reset);
  register_command("endpoint_print_status", &TimingEndpointController::do_endpoint_print_status);
  register_command("endpoint_print_timestamp", &TimingEndpointController::do_endpoint_print_timestamp);
}

void
TimingEndpointController::do_configure(const nlohmann::json& obj)
{
  timingendpointcontroller::from_json(obj,m_cfg);
  
  TLOG() << get_name() << " conf: managed endpoint, device: " << m_cfg.device;
}

void
TimingEndpointController::construct_endpoint_hw_cmd(timingcmd::TimingHwCmd& hw_cmd, const std::string& cmd_id) {
  hw_cmd.id = cmd_id;
  hw_cmd.device = m_cfg.device;
}

void
TimingEndpointController::do_endpoint_io_reset(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_endpoint_hw_cmd(hw_cmd, "endpoint_io_reset");
  send_hw_cmd(hw_cmd);
}

void
TimingEndpointController::do_endpoint_enable(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_endpoint_hw_cmd(hw_cmd, "endpoint_enable");
  send_hw_cmd(hw_cmd);
}

void
TimingEndpointController::do_endpoint_disable(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_endpoint_hw_cmd(hw_cmd, "endpoint_disable");
  send_hw_cmd(hw_cmd);
}

void
TimingEndpointController::do_endpoint_reset(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_endpoint_hw_cmd(hw_cmd, "endpoint_reset");
  send_hw_cmd(hw_cmd);
}

void
TimingEndpointController::do_endpoint_print_timestamp(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_endpoint_hw_cmd(hw_cmd, "endpoint_print_timestamp");
  send_hw_cmd(hw_cmd);
}

void
TimingEndpointController::do_endpoint_print_status(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_endpoint_hw_cmd(hw_cmd, "endpoint_print_status");
  send_hw_cmd(hw_cmd);
}

} // namespace timinglibs 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timinglibs::TimingEndpointController)

// Local Variables:
// c-basic-offset: 2
// End:

/**
 * @file TimingEndpointController.cpp TimingEndpointController class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingEndpointController.hpp"

#include "timing/timingendpointcontroller/Structs.hpp"
#include "timing/timingendpointcontroller/Nljs.hpp"

#include "timing/timingcmd/Structs.hpp"
#include "timing/timingcmd/Nljs.hpp"

#include "CommonIssues.hpp"

#include "appfwk/DAQModuleHelper.hpp"
#include "appfwk/cmd/Nljs.hpp"

#include "ers/ers.h"
#include "TRACE/trace.h"

#include <chrono>
#include <cstdlib>
#include <thread>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

TimingEndpointController::TimingEndpointController(const std::string& name)
  : dunedaq::timing::TimingController(name)
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
  
  ERS_INFO(get_name() << " conf: managed endpoint, device: " << m_cfg.device);
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

} // namespace timing 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timing::TimingEndpointController)

// Local Variables:
// c-basic-offset: 2
// End:

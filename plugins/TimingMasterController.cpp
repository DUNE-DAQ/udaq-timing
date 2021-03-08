/**
 * @file TimingMasterController.cpp TimingMasterController class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingMasterController.hpp"

#include "timinglibs/timingmastercontroller/Structs.hpp"
#include "timinglibs/timingmastercontroller/Nljs.hpp"

#include "timinglibs/timingcmd/Structs.hpp"
#include "timinglibs/timingcmd/Nljs.hpp"

#include "appfwk/DAQModuleHelper.hpp"
#include "appfwk/cmd/Nljs.hpp"

#include "ers/Issue.hpp"

#include <chrono>
#include <cstdlib>
#include <thread>
#include <string>
#include <vector>

namespace dunedaq {
namespace timinglibs {

TimingMasterController::TimingMasterController(const std::string& name)
  : dunedaq::timinglibs::TimingController(name)
{
  register_command("conf", &TimingMasterController::do_configure);
  register_command("start", &TimingMasterController::do_start);
  register_command("stop",  &TimingMasterController::do_stop);
  
  // timing commands
  register_command("master_io_reset", &TimingMasterController::do_master_io_reset);
  register_command("master_set_timestamp", &TimingMasterController::do_master_set_timestamp);
  register_command("master_print_status", &TimingMasterController::do_master_print_status);
}

void
TimingMasterController::do_configure(const nlohmann::json& obj)
{
  timingmastercontroller::from_json(obj, m_cfg);

  TLOG() << get_name() << "conf: managed device: " << m_cfg.device;
}

void
TimingMasterController::construct_master_hw_cmd(timingcmd::TimingHwCmd& hw_cmd, const std::string& cmd_id) {
  hw_cmd.id = cmd_id;
  hw_cmd.device = m_cfg.device;
}

void
TimingMasterController::do_master_io_reset(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_master_hw_cmd(hw_cmd, "master_io_reset");
  send_hw_cmd(hw_cmd);
}

void
TimingMasterController::do_master_set_timestamp(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_master_hw_cmd(hw_cmd, "master_set_timestamp");
  send_hw_cmd(hw_cmd);
}

void
TimingMasterController::do_master_print_status(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_master_hw_cmd(hw_cmd, "master_print_status");
  send_hw_cmd(hw_cmd);
}

} // namespace timinglibs 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timinglibs::TimingMasterController)

// Local Variables:
// c-basic-offset: 2
// End:

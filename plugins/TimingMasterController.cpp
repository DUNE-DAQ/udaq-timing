/**
 * @file TimingMasterController.cpp TimingMasterController class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingMasterController.hpp"

#include "timing/timingmastercontroller/Structs.hpp"
#include "timing/timingmastercontroller/Nljs.hpp"

#include "timing/timingcmd/Structs.hpp"
#include "timing/timingcmd/Nljs.hpp"

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

TimingMasterController::TimingMasterController(const std::string& name)
  : dunedaq::timing::TimingController(name)
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

  ERS_LOG( get_name() << "conf: managed device: " << m_cfg.device );
}

void
TimingMasterController::do_master_io_reset(const nlohmann::json&)
{
  send_hw_cmd(m_cfg.device, "master_io_reset");
}

void
TimingMasterController::do_master_set_timestamp(const nlohmann::json&)
{
  send_hw_cmd(m_cfg.device, "master_set_timestamp");
}

void
TimingMasterController::do_master_print_status(const nlohmann::json&)
{
  send_hw_cmd(m_cfg.device, "master_print_status");
}

} // namespace timing 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timing::TimingMasterController)

// Local Variables:
// c-basic-offset: 2
// End:

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

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "TimingMasterController" // NOLINT

namespace dunedaq {
namespace timing {

TimingMasterController::TimingMasterController(const std::string& name)
  : dunedaq::timing::TimingController(name)
{
  register_command("conf", &TimingMasterController::do_configure);
  register_command("start", &TimingMasterController::do_start);
  register_command("stop",  &TimingMasterController::do_stop);
  register_command("master_reset", &TimingMasterController::do_masterReset);
  register_command("master_set_timestamp", &TimingMasterController::do_masterSetTimestamp);
  register_command("master_print_status", &TimingMasterController::do_masterPrintStatus);
}

void
TimingMasterController::do_configure(const nlohmann::json& obj)
{
  hwCmdId_ = "mastercmd";

  timingmastercontroller::from_json(obj,cfg_);

  TLOG(TLVL_TRACE) << get_name() << "conf: managed device: " << cfg_.device;
}

void
TimingMasterController::do_masterReset(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "reset");
}

void
TimingMasterController::do_masterSetTimestamp(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "set_timestamp");
}

void
TimingMasterController::do_masterPrintStatus(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "print_status");
}

} // namespace timing 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timing::TimingMasterController)

// Local Variables:
// c-basic-offset: 2
// End:

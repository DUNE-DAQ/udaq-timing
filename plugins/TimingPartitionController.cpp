/**
 * @file TimingPartitionController.cpp TimingPartitionController class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingPartitionController.hpp"

#include "timing/timingpartitioncontroller/Structs.hpp"
#include "timing/timingpartitioncontroller/Nljs.hpp"

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

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "TimingPartitionController" // NOLINT

namespace dunedaq {
namespace timing {

TimingPartitionController::TimingPartitionController(const std::string& name)
  : dunedaq::timing::TimingController(name)
{
  register_command("conf", &TimingPartitionController::do_configure);
  //register_command("start", &TimingPartitionController::do_start);
  //register_command("stop",  &TimingPartitionController::do_stop);
  register_command("partition_configure", &TimingPartitionController::do_partitionConfigure);
  register_command("partition_enable", &TimingPartitionController::do_partitionEnable);
  register_command("partition_disable", &TimingPartitionController::do_partitionDisable);
  register_command("partition_start", &TimingPartitionController::do_partitionStart);
  register_command("partition_stop", &TimingPartitionController::do_partitionStop);
  register_command("partition_enable_triggers", &TimingPartitionController::do_partitionEnableTriggers);
  register_command("partition_disable_triggers", &TimingPartitionController::do_partitionDisableTriggers);
  register_command("partition_print_status", &TimingPartitionController::do_partitionPrintStatus);
}

void
TimingPartitionController::do_configure(const nlohmann::json& obj)
{
  hwCmdId_ = "partitioncmd";

  timingpartitioncontroller::from_json(obj,cfg_);
  
  TLOG(TLVL_TRACE) << get_name() << " conf: managed partition, device: " << cfg_.partId << ", " << cfg_.device;
}

void
TimingPartitionController::do_partitionConfigure(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "configure");
}

void
TimingPartitionController::do_partitionEnable(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "enable");
}

void
TimingPartitionController::do_partitionDisable(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "disable");
}

void
TimingPartitionController::do_partitionStart(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "start");
}

void
TimingPartitionController::do_partitionStop(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "stop");
}

void
TimingPartitionController::do_partitionEnableTriggers(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "enable_triggers");
}

void
TimingPartitionController::do_partitionDisableTriggers(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "disable_triggers");
}

void
TimingPartitionController::do_partitionPrintStatus(const nlohmann::json&)
{
  sendHwCmd(cfg_.device, "print_status");
}

} // namespace timing 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timing::TimingPartitionController)

// Local Variables:
// c-basic-offset: 2
// End:

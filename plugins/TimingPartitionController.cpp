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

namespace dunedaq {
namespace timing {

TimingPartitionController::TimingPartitionController(const std::string& name)
  : dunedaq::timing::TimingController(name)
{
  register_command("conf", &TimingPartitionController::do_configure);
  register_command("start", &TimingPartitionController::do_start);
  register_command("stop",  &TimingPartitionController::do_stop);
  register_command("partition_configure", &TimingPartitionController::do_partition_configure);
  register_command("partition_enable", &TimingPartitionController::do_partition_enable);
  register_command("partition_disable", &TimingPartitionController::do_partition_disable);
  register_command("partition_start", &TimingPartitionController::do_partition_start);
  register_command("partition_stop", &TimingPartitionController::do_partition_stop);
  register_command("partition_enable_triggers", &TimingPartitionController::do_partition_enable_triggers);
  register_command("partition_disable_triggers", &TimingPartitionController::do_partition_disable_triggers);
  register_command("partition_print_status", &TimingPartitionController::do_partition_print_status);
}

void
TimingPartitionController::do_configure(const nlohmann::json& obj)
{
  m_hw_cmd_id_ = "partitioncmd";

  timingpartitioncontroller::from_json(obj,cfg_);
  
  ERS_LOG( get_name() << " conf: managed partition, device: " << cfg_.device << ", part id: " << cfg_.partId );
}

void
TimingPartitionController::do_partition_configure(const nlohmann::json&)
{
  send_hw_cmd(cfg_.device, "configure");
}

void
TimingPartitionController::do_partition_enable(const nlohmann::json&)
{
  send_hw_cmd(cfg_.device, "enable");
}

void
TimingPartitionController::do_partition_disable(const nlohmann::json&)
{
  send_hw_cmd(cfg_.device, "disable");
}

void
TimingPartitionController::do_partition_start(const nlohmann::json&)
{
  send_hw_cmd(cfg_.device, "start");
}

void
TimingPartitionController::do_partition_stop(const nlohmann::json&)
{
  send_hw_cmd(cfg_.device, "stop");
}

void
TimingPartitionController::do_partition_enable_triggers(const nlohmann::json&)
{
  send_hw_cmd(cfg_.device, "enable_triggers");
}

void
TimingPartitionController::do_partition_disable_triggers(const nlohmann::json&)
{
  send_hw_cmd(cfg_.device, "disable_triggers");
}

void
TimingPartitionController::do_partition_print_status(const nlohmann::json&)
{
  send_hw_cmd(cfg_.device, "print_status");
}

} // namespace timing 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timing::TimingPartitionController)

// Local Variables:
// c-basic-offset: 2
// End:

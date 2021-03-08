/**
 * @file TimingPartitionController.cpp TimingPartitionController class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingPartitionController.hpp"

#include "timinglibs/timingpartitioncontroller/Structs.hpp"
#include "timinglibs/timingpartitioncontroller/Nljs.hpp"

#include "timinglibs/timingcmd/Structs.hpp"
#include "timinglibs/timingcmd/Nljs.hpp"

#include "TimingIssues.hpp"

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

TimingPartitionController::TimingPartitionController(const std::string& name)
  : dunedaq::timinglibs::TimingController(name)
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
  timingpartitioncontroller::from_json(obj, m_cfg);
  
  TLOG() << get_name() << " conf: managed partition, device: " << m_cfg.device << ", part id: " << m_cfg.partition_id;
}

void
TimingPartitionController::construct_partition_hw_cmd(timingcmd::TimingHwCmd& hw_cmd, const std::string& cmd_id)
{
  timingcmd::TimingPartitionCmdPayload cmd_payload;
  cmd_payload.partition_id = m_cfg.partition_id;
  timingcmd::to_json(hw_cmd.payload, cmd_payload);

  hw_cmd.id = cmd_id;
  hw_cmd.device = m_cfg.device;
}
void
TimingPartitionController::do_partition_configure(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_partition_hw_cmd(hw_cmd, "partition_configure");
  send_hw_cmd(hw_cmd);
}

void
TimingPartitionController::do_partition_enable(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_partition_hw_cmd(hw_cmd, "partition_enable");
  send_hw_cmd(hw_cmd);
}

void
TimingPartitionController::do_partition_disable(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_partition_hw_cmd(hw_cmd, "partition_disable");
  send_hw_cmd(hw_cmd);
}

void
TimingPartitionController::do_partition_start(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_partition_hw_cmd(hw_cmd, "partition_start");
  send_hw_cmd(hw_cmd);
}

void
TimingPartitionController::do_partition_stop(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_partition_hw_cmd(hw_cmd, "partition_stop");
  send_hw_cmd(hw_cmd);
}

void
TimingPartitionController::do_partition_enable_triggers(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_partition_hw_cmd(hw_cmd, "partition_enable_triggers");
  send_hw_cmd(hw_cmd);
}

void
TimingPartitionController::do_partition_disable_triggers(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_partition_hw_cmd(hw_cmd, "partition_disable_triggers");
  send_hw_cmd(hw_cmd);
}

void
TimingPartitionController::do_partition_print_status(const nlohmann::json&)
{
  timingcmd::TimingHwCmd hw_cmd;
  construct_partition_hw_cmd(hw_cmd, "partition_print_status");
  send_hw_cmd(hw_cmd);
}

} // namespace timinglibs 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timinglibs::TimingPartitionController)

// Local Variables:
// c-basic-offset: 2
// End:

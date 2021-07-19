/**
 * @file TimingController.cpp TimingController class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingController.hpp"

#include "timinglibs/timingcmd/Nljs.hpp"
#include "timinglibs/timingcmd/Structs.hpp"

#include "timinglibs/TimingIssues.hpp"

#include "appfwk/cmd/Nljs.hpp"

#include "ers/Issue.hpp"
#include "logging/Logging.hpp"

#include <chrono>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

namespace dunedaq {
namespace timinglibs {

TimingController::TimingController(const std::string& name, uint number_hw_commands)
  : dunedaq::appfwk::DAQModule(name)
  , m_hw_command_out_queue(nullptr)
  , m_hw_cmd_out_queue_timeout(100)
  , m_timing_device("")
  , m_number_hw_commands(number_hw_commands)
  , m_sent_hw_command_counters(m_number_hw_commands)
{
  for (auto it = m_sent_hw_command_counters.begin(); it != m_sent_hw_command_counters.end(); ++it) {
    it->atomic.store(0);
  }
}

void
TimingController::init(const nlohmann::json& init_data)
{
  // set up queues
  auto qinfos = init_data.get<appfwk::app::QueueInfos>();
  for (const auto& qi : qinfos) {
    if (!qi.name.compare("hardware_commands_out")) {
      try {
        m_hw_command_out_queue.reset(new sink_t(qi.inst));
      } catch (const ers::Issue& excpt) {
        throw InvalidQueueFatalError(ERS_HERE, get_name(), qi.name, excpt);
      }
    }
  }
}

void
TimingController::do_start(const nlohmann::json&)
{
  // Timing commands are processed even before a start command. Counters may therefore lose counts after a start.
  // reset counters
  for (auto it = m_sent_hw_command_counters.begin(); it != m_sent_hw_command_counters.end(); ++it) {
    it->atomic.store(0);
  }
}

void
TimingController::do_stop(const nlohmann::json&)
{}

void
TimingController::send_hw_cmd(const timingcmd::TimingHwCmd& hw_cmd)
{
  std::string thisQueueName = m_hw_command_out_queue->get_name();
  try {
    m_hw_command_out_queue->push(hw_cmd, m_hw_cmd_out_queue_timeout);
  } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
    std::ostringstream oss_warn;
    oss_warn << "push to output queue \"" << thisQueueName << "\"";
    ers::warning(dunedaq::appfwk::QueueTimeoutExpired(
      ERS_HERE,
      get_name(),
      oss_warn.str(),
      std::chrono::duration_cast<std::chrono::milliseconds>(m_hw_cmd_out_queue_timeout).count()));
  }
}

} // namespace timinglibs
} // namespace dunedaq

// Local Variables:
// c-basic-offset: 2
// End:

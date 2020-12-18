/**
 * @file TimingController.cpp TimingController class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingController.hpp"

#include "udaqtiming/timingcmd/Structs.hpp"
#include "udaqtiming/timingcmd/Nljs.hpp"

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
#define TRACE_NAME "TimingController" // NOLINT

namespace dunedaq {
namespace udaqtiming {

TimingController::TimingController(const std::string& name)
  : dunedaq::appfwk::DAQModule(name)
  , hwCommandOutQueue_(nullptr)
  , hwCmdOutQueueTimeout_(100)
  , hwCmdId_("")
{
}

void
TimingController::init( const data_t& obj)
{
  // set up queues
  auto qi = appfwk::qindex(obj, {"hardware_commands_out"});
  try
  {
    hwCommandOutQueue_.reset(new sink_t(qi["hardware_commands_out"].inst));
  }
  catch (const ers::Issue& excpt)
  {
    throw InvalidQueueFatalError(ERS_HERE, get_name(), "hardware_commands_out", excpt);
  }
}

//void
//TimingController::do_start(const nlohmann::json& obj)
//{
//  //TLOG(TLVL_INFO) << get_name() << ": Entering do_start() method";
//  //TLOG(TLVL_INFO) << get_name() << ": Exiting do_start() method";
//}
//
//void
//TimingController::do_stop(const nlohmann::json& obj)
//{
//  //TLOG(TLVL_INFO) << get_name() << ": Entering do_stop() method";
//  //TLOG(TLVL_INFO) << get_name() << ": Exiting do_stop() method";
//}

void
TimingController::sendHwCmd(const std::string& device, const timingcmd::TimingCmdId& cmdId)
{
  timingcmd::TimingCmd cmd;
  cmd.id = cmdId;
  cmd.device = device;

  timingcmd::TimingHwCmd hwCmd;
  hwCmd.id = hwCmdId_;
  hwCmd.cmd = cmd;

  std::string thisQueueName = hwCommandOutQueue_->get_name();
  try
  {
    hwCommandOutQueue_->push(hwCmd, hwCmdOutQueueTimeout_);
  }
  catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt)
  {
    std::ostringstream oss_warn;
    oss_warn << "push to output queue \"" << thisQueueName << "\"";
    ers::warning(dunedaq::appfwk::QueueTimeoutExpired(ERS_HERE, get_name(), oss_warn.str(),
      std::chrono::duration_cast<std::chrono::milliseconds>(hwCmdOutQueueTimeout_).count()));
  }
}

} // namespace udaqtiming 
} // namespace dunedaq

// Local Variables:
// c-basic-offset: 2
// End:

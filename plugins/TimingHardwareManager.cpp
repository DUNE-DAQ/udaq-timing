/**
 * @file TimingHardwareManager.cpp TimingHardwareManager class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "TimingHardwareManager.hpp"

#include "timing/timinghardwaremanager/Structs.hpp"
#include "timing/timinghardwaremanager/Nljs.hpp"

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
#include <memory>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "TimingHardwareManager" // NOLINT

namespace dunedaq {
namespace timing {

TimingHardwareManager::TimingHardwareManager(const std::string& name)
  : dunedaq::appfwk::DAQModule(name)
  , thread_(std::bind(&TimingHardwareManager::do_work, this, std::placeholders::_1))
  , hwCommandInQueue_(nullptr)
  , queueTimeout_(100)
  , connectionManager_(nullptr)
{
  register_command("start", &TimingHardwareManager::do_start);
  register_command("conf", &TimingHardwareManager::do_configure);
  register_command("stop",  &TimingHardwareManager::do_stop);
  
  register_timing_hw_command("mastercmd", &TimingHardwareManager::executeMasterCommand);
  register_timing_hw_command("partitioncmd", &TimingHardwareManager::executePartitionCommand);
}

void TimingHardwareManager::init(const nlohmann::json& obj)
{  
  // set up queues
	auto qi = appfwk::qindex(obj, {"hardware_commands_in"});
	try
	{
		hwCommandInQueue_.reset(new source_t(qi["hardware_commands_in"].inst));
	}
	catch (const ers::Issue& excpt)
	{
		throw InvalidQueueFatalError(ERS_HERE, get_name(), "hardware_commands_in", excpt);
	}
}

void
TimingHardwareManager::do_configure(const nlohmann::json& obj)
{
  timinghardwaremanager::from_json(obj,cfg_);
  
  connectinsFile_ = cfg_.connectionsFile;
  
  ERS_INFO( get_name() << "conf: con. file before env var expansion: " << connectinsFile_);
  resolve_environment_variables(connectinsFile_);
  ERS_INFO( get_name() << "conf: con. file after env var expansion:  " << connectinsFile_);

  // uhal log level to be passed in as parameter?
  uhal::setLogLevelTo(uhal::Notice());  
  connectionManager_ = std::make_unique< uhal::ConnectionManager >("file://"+connectinsFile_);
}

void
TimingHardwareManager::do_start(const nlohmann::json&)
{
  thread_.start_working_thread();
  ERS_LOG(get_name() << " successfully started");
}

void
TimingHardwareManager::do_stop(const nlohmann::json&)
{
  thread_.stop_working_thread();
  ERS_LOG(get_name() << " successfully stopped");
}

void
TimingHardwareManager::do_work(std::atomic<bool>& running_flag)
{
  int receivedCommandCount = 0;
  int acceptedCommandCount = 0;
  int rejectedCommandCount = 0;


  while (running_flag.load()) 
  {
    timingcmd::TimingHwCmd lTimingHwCmd;

    try
    {
      hwCommandInQueue_->pop(lTimingHwCmd, queueTimeout_);
    }
    catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt)
    {
      // it is perfectly reasonable that there might be no commands in the queue 
      // some fraction of the times that we check, so we just continue on and try again
      continue;
    }
    
    ++receivedCommandCount;

    TLOG(TLVL_TRACE) << get_name() << ": Received hardware command #" << receivedCommandCount << ", it is of type: " << lTimingHwCmd.id;

    if (auto cmd = timingHwCmdMap_.find(lTimingHwCmd.id); cmd != timingHwCmdMap_.end()) {
      std::invoke(cmd->second, lTimingHwCmd.cmd);
      ++acceptedCommandCount;
    } else {
      ERS_LOG(get_name() << ": Invalid hw cmd: " << lTimingHwCmd.id);
      ++rejectedCommandCount;
    }
  }

  std::ostringstream oss_summ;
  oss_summ << ": Exiting do_work() method, received " << receivedCommandCount << " commands";
  ers::info(ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
}

void
TimingHardwareManager::executeMasterCommand(const timingcmd::TimingCmd& cmd)
{
  auto masterDevice = connectionManager_->getDevice(cmd.device);
  auto masterDesign = masterDevice.getNode<pdt::PDIMasterDesign<pdt::TLUIONode>>("");

  std::string cmdId = cmd.id;

	if (cmdId == "reset") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << " reset";
    masterDesign.reset();
	} else if (cmdId == "set_timestamp") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << " set timestamp";
    masterDesign.getMasterNode().syncTimestamp();
  } else if (cmdId == "print_status") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << " print status";
    TLOG(TLVL_TRACE) << std::endl << masterDesign.getStatus();
  } else {
		ERS_LOG(get_name() << ": unrecognised (master) device command: " << cmd.id);
	}
}

void
TimingHardwareManager::executePartitionCommand(const timingcmd::TimingCmd& cmd)
{
  auto masterDevice = connectionManager_->getDevice(cmd.device);
  auto masterDesign = masterDevice.getNode<pdt::PDIMasterDesign<pdt::TLUIONode>>("");
  auto partition = masterDesign.getMasterNode().getPartitionNode(0);

  std::string cmdId = cmd.id;

  int partId = 0;

  if (cmdId == "configure") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << ", partition: " << partId << ": configure";
    uint32_t fakeMask = (0x1 << partId);
    uint32_t trigMask = (0xf << 4) | fakeMask;

    partition.reset(); 
    partition.configure(trigMask, true, true);
    partition.enable();

  } else if (cmdId == "enable") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << ", partition: " << partId << ": enable";
    partition.enable();
  } else if (cmdId == "disable") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << ", partition: " << partId << ": disable";
    partition.enable(false);
  } else if (cmdId == "start") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << ", partition: " << partId << ": start";
    partition.start();
  } else if (cmdId == "stop") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << ", partition: " << partId << ": stop";
    partition.stop();
  } else if (cmdId == "enable_triggers") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << ", partition: " << partId << ": enable triggers";
    partition.enableTriggers(true);
  } else if (cmdId == "disable_triggers") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << ", partition: " << partId << ": disable triggers";
    partition.enableTriggers(false);
  } else if (cmdId == "print_status") {
    TLOG(TLVL_TRACE) << get_name() << ": " << cmd.device << ", partition: " << partId << ": print status";
    TLOG(TLVL_TRACE) << std::endl << partition.getStatus();
  } else {
    ERS_LOG(get_name() << ": unrecognised partition command: " << cmd.id);
  }
}

} // namespace timing 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timing::TimingHardwareManager)

// Local Variables:
// c-basic-offset: 2
// End:

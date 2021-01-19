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

#include <chrono>
#include <cstdlib>
#include <thread>
#include <string>
#include <vector>
#include <memory>

namespace dunedaq {
namespace timing {

TimingHardwareManager::TimingHardwareManager(const std::string& name)
  : dunedaq::appfwk::DAQModule(name)
  , thread_(std::bind(&TimingHardwareManager::do_work, this, std::placeholders::_1))
  , m_hw_command_in_queue_(nullptr)
  , m_queue_timeout_(100)
  , m_connection_manager_(nullptr)
{
  register_command("start", &TimingHardwareManager::do_start);
  register_command("conf", &TimingHardwareManager::do_configure);
  register_command("stop",  &TimingHardwareManager::do_stop);
  
  register_timing_hw_command("mastercmd", &TimingHardwareManager::execute_master_command);
  register_timing_hw_command("partitioncmd", &TimingHardwareManager::execute_partition_command);
  register_timing_hw_command("endpointcmd", &TimingHardwareManager::execute_endpoint_command);
}

void TimingHardwareManager::init(const nlohmann::json& obj)
{  
  // set up queues
	auto qi = appfwk::qindex(obj, {"hardware_commands_in"});
	try
	{
		m_hw_command_in_queue_.reset(new source_t(qi["hardware_commands_in"].inst));
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
  
  m_connections_file_ = cfg_.connectionsFile;
  
  ERS_INFO( get_name() << "conf: con. file before env var expansion: " << m_connections_file_);
  resolve_environment_variables(m_connections_file_);
  ERS_INFO( get_name() << "conf: con. file after env var expansion:  " << m_connections_file_);

  // uhal log level to be passed in as parameter?
  uhal::setLogLevelTo(uhal::Notice());  
  m_connection_manager_ = std::make_unique< uhal::ConnectionManager >("file://"+m_connections_file_);
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
  int received_command_counts = 0;
  int accepted_command_counts = 0;
  int rejected_command_counts = 0;


  while (running_flag.load()) 
  {
    timingcmd::TimingHwCmd timing_hw_cmd;

    try
    {
      m_hw_command_in_queue_->pop(timing_hw_cmd, m_queue_timeout_);
    }
    catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt)
    {
      // it is perfectly reasonable that there might be no commands in the queue 
      // some fraction of the times that we check, so we just continue on and try again
      continue;
    }
    
    ++received_command_counts;

    ERS_LOG( get_name() << ": Received hardware command #" << received_command_counts << ", it is of type: " << timing_hw_cmd.id);

    if (auto cmd = m_timing_hw_cmd_map_.find(timing_hw_cmd.id); cmd != m_timing_hw_cmd_map_.end()) {
      std::invoke(cmd->second, timing_hw_cmd.cmd);
      ++accepted_command_counts;
    } else {
      ERS_LOG(get_name() << ": Invalid hw cmd: " << timing_hw_cmd.id);
      ++rejected_command_counts;
    }
  }

  std::ostringstream oss_summ;
  oss_summ << ": Exiting do_work() method, received " << received_command_counts << " commands";
  ers::info(ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
}

void
TimingHardwareManager::execute_master_command(const timingcmd::TimingCmd& cmd)
{
  auto master_device = m_connection_manager_->getDevice(cmd.device);
  auto master_design = master_device.getNode<pdt::PDIMasterDesign<pdt::TLUIONode>>("");

  std::string cmd_id = cmd.id;
  
  if (cmd_id == "reset") {
    ERS_LOG( get_name() << ": " << cmd.device << " reset" );
    master_design.reset();
	} else if (cmd_id == "set_timestamp") {
    ERS_LOG( get_name() << ": " << cmd.device << " set timestamp" );
    master_design.getMasterNode().syncTimestamp();
  } else if (cmd_id == "print_status") {
    ERS_LOG( get_name() << ": " << cmd.device << " print status" );
    ERS_INFO( std::endl << master_design.getStatus() );
  } else {
		ERS_LOG(get_name() << ": unrecognised (master) device command: " << cmd.id);
	}
}

void
TimingHardwareManager::execute_partition_command(const timingcmd::TimingCmd& cmd)
{
  auto master_device = m_connection_manager_->getDevice(cmd.device);
  auto master_design = master_device.getNode<pdt::PDIMasterDesign<pdt::TLUIONode>>("");
  auto partition = master_design.getMasterNode().getPartitionNode(0);

  std::string cmd_id = cmd.id;

  int partition_id = 0;

  if (cmd_id == "configure") {
    ERS_LOG( get_name() << ": " << cmd.device << ", partition: " << partition_id << ": configure" );
    uint32_t fake_mask = (0x1 << partition_id);
    uint32_t trig_mask = (0xf << 4) | fake_mask;

    partition.reset(); 
    partition.configure(trig_mask, true, true);
    partition.enable();

  } else if (cmd_id == "enable") {
    ERS_LOG( get_name() << ": " << cmd.device << ", partition: " << partition_id << ": enable" );
    partition.enable();
  } else if (cmd_id == "disable") {
    ERS_LOG( get_name() << ": " << cmd.device << ", partition: " << partition_id << ": disable" );
    partition.enable(false);
  } else if (cmd_id == "start") {
    ERS_LOG( get_name() << ": " << cmd.device << ", partition: " << partition_id << ": start" );
    partition.start();
  } else if (cmd_id == "stop") {
    ERS_LOG( get_name() << ": " << cmd.device << ", partition: " << partition_id << ": stop" );
    partition.stop();
  } else if (cmd_id == "enable_triggers") {
    ERS_LOG( get_name() << ": " << cmd.device << ", partition: " << partition_id << ": enable triggers" );
    partition.enableTriggers(true);
  } else if (cmd_id == "disable_triggers") {
    ERS_LOG( get_name() << ": " << cmd.device << ", partition: " << partition_id << ": disable triggers" );
    partition.enableTriggers(false);
  } else if (cmd_id == "print_status") {
    ERS_LOG( get_name() << ": " << cmd.device << ", partition: " << partition_id << ": print status" );
    ERS_INFO( std::endl << partition.getStatus() );
  } else {
    ERS_LOG(get_name() << ": unrecognised partition command: " << cmd.id);
  }
}

void
TimingHardwareManager::execute_endpoint_command(const timingcmd::TimingCmd& cmd)
{
  auto endpoint_device = m_connection_manager_->getDevice(cmd.device);
  auto endpoint_design = endpoint_device.getNode<pdt::EndpointDesign<pdt::FMCIONode>>("");

  std::string cmd_id = cmd.id;

  if (cmd_id == "reset") {
    ERS_LOG( get_name() << ": ept device: " << cmd.device << " reset");
    endpoint_design.reset();
  } else if (cmd_id == "enable") {
    ERS_LOG( get_name() << ": ept device: " << cmd.device << " enable");
    endpoint_design.getEndpointNode(0).enable();
  } else if (cmd_id == "disable") {
    ERS_LOG( get_name() << ": ept device: " << cmd.device << " disable");
    endpoint_design.getEndpointNode(0).disable();
  } else if (cmd_id == "print_timestamp") {
    ERS_LOG( get_name() << ": ept " << cmd.device << " print timestamp");
    uint64_t timestamp = endpoint_design.getEndpointNode(0).readTimestamp();
    ERS_INFO( get_name() << ": ept device:" << cmd.device << " timestamp: " << pdt::formatRegValue(timestamp) );
  } else if (cmd_id == "print_status") {
    ERS_LOG( get_name() << ": ept device: " << cmd.device << " print status");
    ERS_INFO( std::endl << endpoint_design.getStatus() );
  } else {
    ERS_LOG(get_name() << ": unrecognised (endpoint) device command: " << cmd.id);
  }
}

} // namespace timing 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::timing::TimingHardwareManager)

// Local Variables:
// c-basic-offset: 2
// End:

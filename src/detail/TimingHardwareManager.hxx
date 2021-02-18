namespace dunedaq::timing {

template<class MSTR_DSGN, class EPT_DSGN>
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::TimingHardwareManager(const std::string& name)
  : dunedaq::appfwk::DAQModule(name)
  , thread_(std::bind(&TimingHardwareManager::do_work, this, std::placeholders::_1))
  , m_hw_command_in_queue(nullptr)
  , m_queue_timeout(100)
  , m_connection_manager(nullptr)
{
  // all hardware manager variants will need these commands
  register_command("start", &TimingHardwareManager::do_start);
  register_command("stop",  &TimingHardwareManager::do_stop);
}

template<class MSTR_DSGN, class EPT_DSGN>
void TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::init(const nlohmann::json& obj)
{  
  // set up queues
  auto qi = appfwk::qindex(obj, {"hardware_commands_in"});
  try
  {
    m_hw_command_in_queue.reset(new source_t(qi["hardware_commands_in"].inst));
  }
  catch (const ers::Issue& excpt)
  {
    throw InvalidQueueFatalError(ERS_HERE, get_name(), "hardware_commands_in", excpt);
  }
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::do_start(const nlohmann::json&)
{
  thread_.start_working_thread();
  ERS_LOG(get_name() << " successfully started");
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::do_stop(const nlohmann::json&)
{
  thread_.stop_working_thread();
  ERS_LOG(get_name() << " successfully stopped");
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::do_work(std::atomic<bool>& running_flag)
{
  int received_command_counts = 0;
  int accepted_command_counts = 0;
  int rejected_command_counts = 0;


  while (running_flag.load()) 
  {
    timingcmd::TimingHwCmd timing_hw_cmd;

    try
    {
      m_hw_command_in_queue->pop(timing_hw_cmd, m_queue_timeout);
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
      std::invoke(cmd->second, timing_hw_cmd);
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

template<class MSTR_DSGN, class EPT_DSGN>
template<typename Child>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::register_timing_hw_command(const std::string& name, void (Child::*f)(const timingcmd::TimingHwCmd&))
{
  using namespace std::placeholders;

  bool done = m_timing_hw_cmd_map_.emplace(name, std::bind(f, dynamic_cast<Child*>(this), _1)).second;
  if (!done) {
  	// TODO throw specific error
  	throw dunedaq::appfwk::CommandRegistrationFailed(ERS_HERE, get_name(), name);
  }
}

template<class MSTR_DSGN, class EPT_DSGN>
template<class TIMING_DEV>
const TIMING_DEV&
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::get_timing_device(const std::string& device_name) {
	
  std::lock_guard<std::mutex> hw_device_map_guard(m_hw_device_map_mutex);
  
	if (auto hw_device_entry = m_hw_device_map.find(device_name); hw_device_entry != m_hw_device_map.end()) {
      return hw_device_entry->second->getNode<TIMING_DEV>("");
    } else {
      ERS_LOG(get_name() << ": hw device interface for: " << device_name << " does not exist. I will try to create it.");

      m_hw_device_map.emplace( device_name, std::make_unique<uhal::HwInterface>( m_connection_manager->getDevice(device_name) ) );
	
	   ERS_LOG(get_name() << ": hw device interface for: " << device_name << " successfully created.");
    }
    // TODO catch an an exception and throw a proper issue if device cannot be found
    return m_hw_device_map.find(device_name)->second->getNode<TIMING_DEV>("");
}

// master commands
template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::master_io_reset(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " reset" );
  master_design.reset();
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::master_set_timestamp(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " set timestamp" );
  master_design.get_master_node().sync_timestamp();
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::master_print_status(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " print status" );
  ERS_INFO( std::endl << master_design.get_status() );
}

// partition commands
template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::partition_configure(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  auto partition = master_design.get_master_node().get_partition_node(0);
  
  uint32_t partition_id = 0;

  uint32_t fake_mask = (0x1 << partition_id);
  uint32_t trig_mask = (0xf << 4) | fake_mask;

  bool spill_gate_enabled = true;
  bool event_rate_control = true;

  ERS_LOG( get_name() << ": " << hw_cmd.device << " partition 0 configure" );

  partition.reset(); 
  partition.configure(trig_mask, spill_gate_enabled, event_rate_control);
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::partition_enable(const timingcmd::TimingHwCmd& hw_cmd)
{
  timingcmd::TimingPartitionCmdPayload cmd_payload;
  timingcmd::from_json(hw_cmd.payload, cmd_payload);

  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  auto partition = master_design.get_master_node().get_partition_node(cmd_payload.partition_id);

  ERS_LOG( get_name() << ": " << hw_cmd.device << " partition " <<  cmd_payload.partition_id << " enable" );
  partition.enable(true);
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::partition_disable(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  auto partition = master_design.get_master_node().get_partition_node(0);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " partition 0 disable" );
  partition.enable(false);
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::partition_start(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  auto partition = master_design.get_master_node().get_partition_node(0);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " partition 0 start" );
  partition.start();
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::partition_stop(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  auto partition = master_design.get_master_node().get_partition_node(0);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " partition 0 stop" );
  partition.stop();
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::partition_enable_triggers(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  auto partition = master_design.get_master_node().get_partition_node(0);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " partition 0 start triggers" );
  partition.enable_triggers(true);
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::partition_disable_triggers(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  auto partition = master_design.get_master_node().get_partition_node(0);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " partition 0 stop triggers" );
  partition.enable_triggers(false);
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::partition_print_status(const timingcmd::TimingHwCmd& hw_cmd)
{
  timingcmd::TimingPartitionCmdPayload cmd_payload;
  timingcmd::from_json(hw_cmd.payload, cmd_payload);

  auto master_design = get_timing_device<MSTR_DSGN>(hw_cmd.device);
  auto partition = master_design.get_master_node().get_partition_node(cmd_payload.partition_id);
  
  ERS_LOG( get_name() << ": " << hw_cmd.device << " print partition " << cmd_payload.partition_id << " status" );
  ERS_INFO( std::endl << partition.get_status() );
}

// endpoint commands
template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::endpoint_io_reset(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto endpoint_design = get_timing_device<EPT_DSGN>(hw_cmd.device);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " print status" );
  endpoint_design.get_io_node().reset();
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::endpoint_enable(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto endpoint_design = get_timing_device<EPT_DSGN>(hw_cmd.device);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " enable" );
  endpoint_design.get_endpoint_node(0).enable();
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::endpoint_disable(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto endpoint_design = get_timing_device<EPT_DSGN>(hw_cmd.device);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " disable" );
  endpoint_design.get_endpoint_node(0).disable();
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::endpoint_reset(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto endpoint_design = get_timing_device<EPT_DSGN>(hw_cmd.device);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " reset" );
  endpoint_design.get_endpoint_node(0).reset();
}

template<class MSTR_DSGN, class EPT_DSGN>
void
TimingHardwareManager<MSTR_DSGN, EPT_DSGN>::endpoint_print_status(const timingcmd::TimingHwCmd& hw_cmd)
{
  auto endpoint_design = get_timing_device<EPT_DSGN>(hw_cmd.device);
  ERS_LOG( get_name() << ": " << hw_cmd.device << " print status" );
  ERS_INFO( std::endl << endpoint_design.get_endpoint_node(0).get_status() );
}

} // namespace dunedaq::timing
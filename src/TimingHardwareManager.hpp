/**
 * @file TimingHardwareManager.hpp
 *
 * TimingHardwareManager is a DAQModule implementation that
 * provides the interface to the timing system hardware.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMINGLIBS_SRC_TIMINGHARDWAREMANAGER_HPP_
#define TIMINGLIBS_SRC_TIMINGHARDWAREMANAGER_HPP_

#include "timinglibs/timingcmd/Nljs.hpp"
#include "timinglibs/timingcmd/Structs.hpp"

#include "timinglibs/TimingIssues.hpp"

#include "InfoGatherer.hpp"

#include "timing/FanoutDesign.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQModuleHelper.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "appfwk/app/Nljs.hpp"
#include "appfwk/app/Structs.hpp"

#include "ers/Issue.hpp"
#include "logging/Logging.hpp"

#include "uhal/ConnectionManager.hpp"

#include "timing/EndpointNode.hpp"

#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>

// NOLINTNEXTLINE(build/define_used)
#define ADD_VARIADIC_TEMPLATE_PROCESSOR_DECLARATIONS(FUNCTION_NAME)                                                    \
  template<class DSGN>                                                                                                 \
  void FUNCTION_NAME();                                                                                                \
  template<class DSGN0, class DSGN1, class... DSGNS>                                                                   \
  void FUNCTION_NAME()                                                                                                 \
  {                                                                                                                    \
    FUNCTION_NAME<DSGN0>();                                                                                            \
    FUNCTION_NAME<DSGN1, DSGNS...>();                                                                                  \
  }

namespace dunedaq {
namespace timinglibs {

void
resolve_environment_variables(std::string& input_string)
{
  static std::regex env_var_pattern("\\$\\{([^}]+)\\}");
  std::smatch match;
  while (std::regex_search(input_string, match, env_var_pattern)) {
    const char* s = getenv(match[1].str().c_str());
    const std::string env_var(s == nullptr ? "" : s);
    input_string.replace(match[0].first, match[0].second, env_var);
  }
}

/**
 * @brief TimingHardwareManager creates vectors of ints and writes
 * them to the configured output queues.
 */
class TimingHardwareManager : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief TimingHardwareManager Constructor
   * @param name Instance name for this TimingHardwareManager instance
   */
  explicit TimingHardwareManager(const std::string& name);

  TimingHardwareManager(const TimingHardwareManager&) = delete; ///< TimingHardwareManager is not copy-constructible
  TimingHardwareManager& operator=(const TimingHardwareManager&) =
    delete;                                                ///< TimingHardwareManager is not copy-assignable
  TimingHardwareManager(TimingHardwareManager&&) = delete; ///< TimingHardwareManager is not move-constructible
  TimingHardwareManager& operator=(TimingHardwareManager&&) = delete; ///< TimingHardwareManager is not move-assignable

  void init(const nlohmann::json& init_data) override;

protected:
  // Commands
  virtual void do_configure(const nlohmann::json& obj) = 0;
  virtual void do_start(const nlohmann::json&);
  virtual void do_stop(const nlohmann::json&);
  virtual void do_scrap(const nlohmann::json&);

  // Threading
  dunedaq::appfwk::ThreadHelper thread_;
  virtual void process_hardware_commands(std::atomic<bool>&);

  // Configuration
  using source_t = dunedaq::appfwk::DAQSource<timingcmd::TimingHwCmd>;
  std::unique_ptr<source_t> m_hw_command_in_queue;
  std::chrono::milliseconds m_queue_timeout;
  
  // hardware polling intervals [us]
  uint m_gather_interval;
  uint m_gather_interval_debug;

  // uhal members
  std::string m_connections_file;
  std::string m_uhal_log_level;
  std::unique_ptr<uhal::ConnectionManager> m_connection_manager;
  std::map<std::string, std::unique_ptr<uhal::HwInterface>> m_hw_device_map;
  std::mutex m_hw_device_map_mutex;

  void construct_hw_cmd_name(const timingcmd::TimingHwCmd& hw_cmd, std::string& hw_cmd_name);

  // retrieve top level/design object for a timing device
  template<class TIMING_DEV>
  const TIMING_DEV& get_timing_device(const std::string& device_name);

  // timing hw cmds stuff
  std::map<timingcmd::TimingHwCmdId, std::function<void(const timingcmd::TimingHwCmd&)>> m_timing_hw_cmd_map_;

  template<typename Child>
  void register_timing_hw_command(const std::string& hw_cmd_id,
                                  const std::string& design_type,
                                  void (Child::*f)(const timingcmd::TimingHwCmd&));

  // timing common commands
  template<class DSGN>
  void io_reset(const timingcmd::TimingHwCmd& hw_cmd);

  template<class DSGN>
  void print_status(const timingcmd::TimingHwCmd& hw_cmd);

  // timing master commands
  template<class DSGN>
  void set_timestamp(const timingcmd::TimingHwCmd& hw_cmd);

  // timing partition commands
  template<class DSGN>
  void partition_configure(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void partition_enable(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void partition_disable(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void partition_start(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void partition_stop(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void partition_enable_triggers(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void partition_disable_triggers(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void partition_print_status(const timingcmd::TimingHwCmd& hw_cmd);

  // timing endpoint commands
  template<class DSGN>
  void endpoint_enable(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void endpoint_disable(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void endpoint_reset(const timingcmd::TimingHwCmd& hw_cmd);

  // hsi
  template<class DSGN>
  void hsi_reset(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void hsi_configure(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void hsi_start(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void hsi_stop(const timingcmd::TimingHwCmd& hw_cmd);
  template<class DSGN>
  void hsi_print_status(const timingcmd::TimingHwCmd& hw_cmd);

  // opmon stuff
  std::atomic<uint64_t> m_received_hw_commands_counter; // NOLINT(build/unsigned)
  std::atomic<uint64_t> m_accepted_hw_commands_counter; // NOLINT(build/unsigned)
  std::atomic<uint64_t> m_rejected_hw_commands_counter; // NOLINT(build/unsigned)
  std::atomic<uint64_t> m_failed_hw_commands_counter;   // NOLINT(build/unsigned)

  // monitoring
  std::map<std::string, std::unique_ptr<InfoGathererInterface>> m_info_gatherers;

  template<class INFO, class DSGN>
  void register_info_gatherer(uint gather_interval, const std::string& device_name, int op_mon_level);

  template<class INFO, class DSGN>
  void gather_monitor_data(InfoGatherer<INFO>& gatherer);

  virtual void start_hw_mon_gathering(const std::string& device_name="");
  virtual void stop_hw_mon_gathering(const std::string& device_name="");
};

} // namespace timinglibs
} // namespace dunedaq

#include "detail/TimingHardwareManager.hxx"

#endif // TIMINGLIBS_SRC_TIMINGHARDWAREMANAGER_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

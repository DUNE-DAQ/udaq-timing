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

#ifndef TIMING_SRC_TIMINGHARDWAREMANAGER_HPP_
#define TIMING_SRC_TIMINGHARDWAREMANAGER_HPP_

#include "timing/timingcmd/Structs.hpp"
#include "timing/timingcmd/Nljs.hpp"

#include "CommonIssues.hpp"
#include "ModuleMonitor.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQModuleHelper.hpp"

#include "ers/Issue.h"

#include "uhal/ConnectionManager.hpp"

#include "pdt/PDIMasterDesign.hpp"
#include "pdt/EndpointDesign.hpp"
#include "pdt/EndpointNode.hpp"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <regex>

namespace dunedaq {
namespace timing {

struct MonInfo {
  uint counter;
  MonInfo() 
  {}
  MonInfo(uint cntr) 
  : counter(cntr)
  {}
};

/**
 * @brief TimingHardwareManager creates vectors of ints and writes
 * them to the configured output queues.
 */
template<class MSTR_DSGN, class EPT_DSGN>
class TimingHardwareManager : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief TimingHardwareManager Constructor
   * @param name Instance name for this TimingHardwareManager instance
   */
  explicit TimingHardwareManager(const std::string& name);

  TimingHardwareManager(const TimingHardwareManager&) =
    delete; ///< TimingHardwareManager is not copy-constructible
  TimingHardwareManager& operator=(const TimingHardwareManager&) =
    delete; ///< TimingHardwareManager is not copy-assignable
  TimingHardwareManager(TimingHardwareManager&&) =
    delete; ///< TimingHardwareManager is not move-constructible
  TimingHardwareManager& operator=(TimingHardwareManager&&) =
    delete; ///< TimingHardwareManager is not move-assignable

  void init(const nlohmann::json& obj) override;

protected:
  // Commands
  virtual void do_configure(const data_t& obj) = 0;
  virtual void do_start(const nlohmann::json&);
  virtual void do_stop(const nlohmann::json&);

  // Threading
  dunedaq::appfwk::ThreadHelper thread_;
  virtual void do_work(std::atomic<bool>&);

  // Configuration
  using source_t = dunedaq::appfwk::DAQSource<timingcmd::TimingHwCmd>;
  std::unique_ptr<source_t> m_hw_command_in_queue;
  std::chrono::milliseconds m_queue_timeout;
  
  // uhal members
  std::string m_connections_file;
  std::unique_ptr<uhal::ConnectionManager> m_connection_manager;
  std::map < std::string, std::unique_ptr<uhal::HwInterface> > m_hw_device_map;
  std::mutex m_hw_device_map_mutex;

  // retrieve top level/design object for a timing device
  template<class TIMING_DEV>
  const TIMING_DEV& get_timing_device(const std::string& device_name);
  
  // timing hw cmds stuff
  std::map<timingcmd::TimingHwCmdId, std::function<void(const timingcmd::TimingHwCmd&)>> m_timing_hw_cmd_map_;
  template<typename Child>
  void register_timing_hw_command(const std::string& name, void (Child::*f)(const timingcmd::TimingHwCmd&));

  // timing master commands
  virtual void master_io_reset(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void master_set_timestamp(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void master_print_status(const timingcmd::TimingHwCmd& hw_cmd);

  // timing partition commands
  virtual void partition_configure(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void partition_enable(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void partition_disable(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void partition_start(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void partition_stop(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void partition_enable_triggers(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void partition_disable_triggers(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void partition_print_status(const timingcmd::TimingHwCmd& hw_cmd);

  // timing endpoint commands
  virtual void endpoint_io_reset(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void endpoint_enable(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void endpoint_disable(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void endpoint_reset(const timingcmd::TimingHwCmd& hw_cmd);
  virtual void endpoint_print_status(const timingcmd::TimingHwCmd& hw_cmd);

  // ship mon data off
  virtual void get_info(const nlohmann::json&) = 0;

};

void resolve_environment_variables(std::string& input_string) {
    static std::regex env_var_pattern( "\\$\\{([^}]+)\\}" );
    std::smatch match;
    while ( std::regex_search( input_string, match, env_var_pattern ) ) {
        const char * s = getenv( match[1].str().c_str() );
        const std::string env_var( s == nullptr ? "" : s );
        input_string.replace( match[0].first, match[0].second, env_var );
    }
}

} // namespace timing
} // namespace dunedaq

#include "detail/TimingHardwareManager.hxx"

#endif // TIMING_SRC_TIMINGHARDWAREMANAGER_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

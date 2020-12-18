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

#ifndef TIMING_PLUGINS_TIMINGHARDWAREMANAGER_HPP_
#define TIMING_PLUGINS_TIMINGHARDWAREMANAGER_HPP_

#include "timing/timingcmd/Structs.hpp"
#include "timing/timinghardwaremanager/Structs.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"

#include <ers/Issue.h>

#include "uhal/ConnectionManager.hpp"

#include "pdt/PDIMasterDesign.hpp"

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace dunedaq {
namespace timing {

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

  TimingHardwareManager(const TimingHardwareManager&) =
    delete; ///< TimingHardwareManager is not copy-constructible
  TimingHardwareManager& operator=(const TimingHardwareManager&) =
    delete; ///< TimingHardwareManager is not copy-assignable
  TimingHardwareManager(TimingHardwareManager&&) =
    delete; ///< TimingHardwareManager is not move-constructible
  TimingHardwareManager& operator=(TimingHardwareManager&&) =
    delete; ///< TimingHardwareManager is not move-assignable

  void init(const nlohmann::json& obj) override;

private:
  // Commands
  void do_configure(const data_t& obj);
  void do_start(const nlohmann::json&);
  void do_stop(const nlohmann::json&);

  // Threading
  dunedaq::appfwk::ThreadHelper thread_;
  void do_work(std::atomic<bool>&);

  // Configuration
  timinghardwaremanager::Conf cfg_;
  using source_t = dunedaq::appfwk::DAQSource<timingcmd::TimingHwCmd>;
  std::unique_ptr<source_t> hwCommandInQueue_;
  std::chrono::milliseconds queueTimeout_;

  void executeMasterCommand(const timingcmd::TimingCmd& cmd);
  void executePartitionCommand(const timingcmd::TimingCmd& cmd);

  std::map<timingcmd::TimingHwCmdId, std::function<void(const timingcmd::TimingCmd)>> timingHwCmdMap_;

  template<typename Child>
  void register_timing_hw_command(const std::string& name, void (Child::*f)(const timingcmd::TimingCmd&));

  std::string connectinsFile_;
  std::unique_ptr<uhal::ConnectionManager> connectionManager_;

};
} // namespace timing
} // namespace dunedaq

#include "detail/TimingHardwareManager.hxx"

#endif // TIMING_PLUGINS_TIMINGHARDWAREMANAGER_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

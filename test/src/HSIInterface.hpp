/**
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMINGLIBS_TEST_SRC_HSIINTERFACE_HPP_
#define TIMINGLIBS_TEST_SRC_HSIINTERFACE_HPP_

#include "timinglibs/TimingIssues.hpp"

#include "TimingHardwareManager.hpp"

#include "dfmessages/HSIEvent.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQModuleHelper.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"

#include <ers/Issue.hpp>

#include <bitset>
#include <chrono>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace dunedaq {
namespace timinglibs {

/**
 * @brief HSIInterface generates fake HSIEvent messages
 * and pushes them to the configured output queue.
 */
class HSIInterface : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief HSIInterface Constructor
   * @param name Instance name for this HSIInterface instance
   */
  explicit HSIInterface(const std::string& name, std::function<void(std::atomic<bool>&)> do_work);

  HSIInterface(const HSIInterface&) = delete;            ///< HSIInterface is not copy-constructible
  HSIInterface& operator=(const HSIInterface&) = delete; ///< HSIInterface is not copy-assignable
  HSIInterface(HSIInterface&&) = delete;                 ///< HSIInterface is not move-constructible
  HSIInterface& operator=(HSIInterface&&) = delete;      ///< HSIInterface is not move-assignable

  void init(const nlohmann::json& obj) override;
  void get_info(opmonlib::InfoCollector& ci, int level) override;

protected:
  // Commands
  virtual void do_configure(const nlohmann::json& obj) = 0;
  virtual void do_start(const nlohmann::json& obj);
  virtual void do_stop(const nlohmann::json& obj);
  virtual void do_scrap(const nlohmann::json& obj);

  // Threading
  dunedaq::appfwk::ThreadHelper m_thread;

  // Configuration
  using sink_t = dunedaq::appfwk::DAQSink<dfmessages::HSIEvent>;
  std::unique_ptr<sink_t> m_hsievent_sink;
  std::chrono::milliseconds m_queue_timeout;

  // push events to HSIEvent output queue
  void send_hsi_event(dfmessages::HSIEvent& event);
  std::atomic<uint64_t> m_sent_counter;           // NOLINT(build/unsigned)
  std::atomic<uint64_t> m_failed_to_send_counter; // NOLINT(build/unsigned)
  std::atomic<uint64_t> m_last_sent_timestamp;    // NOLINT(build/unsigned)
};
} // namespace timinglibs
} // namespace dunedaq

#endif // TIMINGLIBS_TEST_SRC_HSIINTERFACE_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

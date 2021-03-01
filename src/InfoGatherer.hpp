/**
 * @file InfoGatherer.hpp
 *
 * InfoGatherer is a DAQModule implementation that
 * provides the a mechanism of collecting and filling monitoring data in a dedicated thread.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_SRC_InfoGatherer_HPP_
#define TIMING_SRC_InfoGatherer_HPP_

#include "ers/Issue.hpp"
#include "logging/Logging.hpp"

#include <functional>
#include <future>
#include <list>
#include <memory>
#include <string>
#include <shared_mutex>

namespace dunedaq {

/**
 * @brief An ERS Issue raised when a threading state error occurs
 */
ERS_DECLARE_ISSUE(timing,                                      // Namespace
                  GatherThreadingIssue,                       // Issue Class Name
                  "Gather Threading Issue detected: " << err, // Message
                  ((std::string)err))                          // Message parameters

namespace timing {

/**
 * @brief InfoGatherer helper class for DAQ module monitor
 * data gathering.
 */
template<class MON_DATA>
class InfoGatherer
{
public:
  /**
   * @brief InfoGatherer Constructor
   * @param gather_data function for data gathering 
   * @param gather_interval interval for data gathering in us
   */
  explicit InfoGatherer(std::function<void(InfoGatherer<MON_DATA>&)> gather_data, uint gather_interval)
    : m_run_gathering(false)
    , m_gathering_thread(nullptr)
    , m_gather_data(gather_data)
    , m_gather_interval(gather_interval)
  {}

  InfoGatherer(const InfoGatherer&) =
    delete; ///< InfoGatherer is not copy-constructible
  InfoGatherer& operator=(const InfoGatherer&) =
    delete; ///< InfoGatherer is not copy-assignable
  InfoGatherer(InfoGatherer&&) =
    delete; ///< InfoGatherer is not move-constructible
  InfoGatherer& operator=(InfoGatherer&&) =
    delete; ///< InfoGatherer is not move-assignable

  /**
   * @brief Start the monitoring thread (which executes the m_gather_data() function)
   * @throws MonitorThreadingIssue if the thread is already running
   */
  void start_gathering_thread(const std::string& name="noname")
  {
    if (run_gathering()) {
      throw GatherThreadingIssue(ERS_HERE,
                           "Attempted to start gathering thread "
                           "when it is already supposed to be running!");
    }
    m_run_gathering = true;
    m_gathering_thread.reset(new std::thread([&] { m_gather_data(*this); }));
    auto handle = m_gathering_thread->native_handle();
    auto rc=pthread_setname_np(handle, name.c_str());
    if(rc !=0) {
       std::ostringstream s;
       s << "The name " << name << " provided for the thread is too long.";
       ers::warning(GatherThreadingIssue(ERS_HERE, s.str()));
    }
  }
  /**
   * @brief Stop the gathering thread
   * @throws GatherThreadingIssue If the thread has not yet been started
   * @throws GatherThreadingIssue If the thread is not in the joinable state
   * @throws GatherThreadingIssue If an exception occurs during thread join
   */
  void stop_gathering_thread()
  {
    if (!run_gathering()) {
      throw GatherThreadingIssue(ERS_HERE,
                           "Attempted to stop gathering thread "
                           "when it is not supposed to be running!");
    }
    m_run_gathering = false;
    if (m_gathering_thread->joinable()) {
      try {
        m_gathering_thread->join();
      } catch (std::system_error const& e) {
        throw GatherThreadingIssue(ERS_HERE, std::string("Error while joining gathering thread, ") + e.what());
      }
    } else {
      throw GatherThreadingIssue(ERS_HERE, "Thread not in joinable state during working thread stop!");
    }
  }

  /**
   * @brief Determine if the thread is currently running
   * @return Whether the thread is currently running
   */
  bool run_gathering() const { return m_run_gathering.load(); }

  void update_monitoring_data(MON_DATA& new_data) {
    std::unique_lock mon_data_lock(m_mon_data_mutex);
    m_mon_data = new_data;
  }

  const MON_DATA get_monitoring_data() const { 

    // TODO, what happens if the m_mon_data has not been filled before?
    std::shared_lock mon_data_lock(m_mon_data_mutex);
    return m_mon_data; 
  }

  void update_gather_interval(uint new_gather_interval) { m_gather_interval.store(new_gather_interval); }

  uint get_gather_interval() const { return m_gather_interval.load(); }

private:
  std::atomic<bool> m_run_gathering;
  std::unique_ptr<std::thread> m_gathering_thread;
  std::function<void(InfoGatherer<MON_DATA>&)> m_gather_data;
  std::atomic<uint> m_gather_interval;
  MON_DATA m_mon_data;
  mutable std::shared_mutex m_mon_data_mutex;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_SRC_InfoGatherer_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

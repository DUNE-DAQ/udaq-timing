/**
 * @file ModuleMonitor.hpp
 *
 * ModuleMonitor is a DAQModule implementation that
 * provides the interface to the timing system hardware.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_SRC_MODULEMONITOR_HPP_
#define TIMING_SRC_MODULEMONITOR_HPP_

#include "ers/Issue.h"
#include "ers/ers.h"

#include <functional>
#include <future>
#include <list>
#include <memory>
#include <string>

namespace dunedaq {

/**
 * @brief An ERS Issue raised when a threading state error occurs
 */
ERS_DECLARE_ISSUE(timing,                                      // Namespace
                  MonitorThreadingIssue,                       // Issue Class Name
                  "Monitor Threading Issue detected: " << err, // Message
                  ((std::string)err))                          // Message parameters

namespace timing {

/**
 * @brief ModuleMonitor helper class for DAQ module monitor
 * data gathering.
 */
template<class MON_DATA>
class ModuleMonitor
{
public:
  /**
   * @brief ModuleMonitor Constructor
   * @param gather_data function for data gathering 
   * @param gather_interval interval for data gathering in us
   */
  explicit ModuleMonitor(std::function<void(std::atomic<bool>&, std::atomic<MON_DATA>&, std::atomic<uint>&)> gather_data)
    : m_monitor_running(false)
    , m_monitoring_thread(nullptr)
    , m_gather_data(gather_data)
    , m_gather_interval(1000)
    , m_mon_data(MON_DATA())
  {}

  ModuleMonitor(const ModuleMonitor&) =
    delete; ///< ModuleMonitor is not copy-constructible
  ModuleMonitor& operator=(const ModuleMonitor&) =
    delete; ///< ModuleMonitor is not copy-assignable
  ModuleMonitor(ModuleMonitor&&) =
    delete; ///< ModuleMonitor is not move-constructible
  ModuleMonitor& operator=(ModuleMonitor&&) =
    delete; ///< ModuleMonitor is not move-assignable

  /**
   * @brief Start the monitoring thread (which executes the m_gather_data() function)
   * @throws MonitorThreadingIssue if the thread is already running
   */
  void start_monitoring_thread(const std::string& name="noname")
  {
    if (thread_running()) {
      throw MonitorThreadingIssue(ERS_HERE,
                           "Attempted to start monitoring thread "
                           "when it is already running!");
    }
    m_monitor_running = true;
    m_monitoring_thread.reset(new std::thread([&] { m_gather_data(std::ref(m_monitor_running), std::ref(m_mon_data), std::ref(m_gather_interval)); }));
    auto handle = m_monitoring_thread->native_handle();
    auto rc=pthread_setname_np(handle, name.c_str());
    if(rc !=0) {
       std::ostringstream s;
       s << "The name " << name << " provided for the thread is too long.";
       ers::warning(MonitorThreadingIssue(ERS_HERE, s.str()));
    }
  }
  /**
   * @brief Stop the monitoring thread
   * @throws MonitorThreadingIssue If the thread has not yet been started
   * @throws MonitorThreadingIssue If the thread is not in the joinable state
   * @throws MonitorThreadingIssue If an exception occurs during thread join
   */
  void stop_monitoring_thread()
  {
    if (!thread_running()) {
      throw MonitorThreadingIssue(ERS_HERE,
                           "Attempted to stop monitoring thread "
                           "when it is not running!");
    }
    m_monitor_running = false;

    if (m_monitoring_thread->joinable()) {
      try {
        m_monitoring_thread->join();
      } catch (std::system_error const& e) {
        throw MonitorThreadingIssue(ERS_HERE, std::string("Error while joining monitoring thread, ") + e.what());
      }
    } else {
      throw MonitorThreadingIssue(ERS_HERE, "Thread not in joinable state during working thread stop!");
    }
  }

  /**
   * @brief Determine if the thread is currently running
   * @return Whether the thread is currently running
   */
  bool thread_running() const { return m_monitor_running.load(); }

  const MON_DATA get_monitoring_data() const { return m_mon_data.load(); }

private:
  std::atomic<bool> m_monitor_running;
  std::unique_ptr<std::thread> m_monitoring_thread;
  std::function<void(std::atomic<bool>&, std::atomic<MON_DATA>&, std::atomic<uint>&)> m_gather_data;
  std::atomic<uint> m_gather_interval;
  std::atomic<MON_DATA> m_mon_data;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_SRC_MODULEMONITOR_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

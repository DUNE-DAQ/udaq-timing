/**
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMINGLIBS_SRC_HSIREADOUT_HPP_
#define TIMINGLIBS_SRC_HSIREADOUT_HPP_

#include "timinglibs/hsireadout/Structs.hpp"
#include "timinglibs/TimingIssues.hpp"

#include "HSIInterface.hpp"

#include "timing/HSINode.hpp"

#include "dfmessages/HSIEvent.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/ThreadHelper.hpp"

#include "uhal/ConnectionManager.hpp"
#include "uhal/utilities/files.hpp"

#include <ers/Issue.hpp>

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <bitset>

namespace dunedaq {
namespace timinglibs {

/**
 * @brief HSIReadout generates fake HSIEvent messages
 * and pushes them to the configured output queue.
 */
class HSIReadout : public dunedaq::timinglibs::HSIInterface
{
public:
  /**
   * @brief HSIReadout Constructor
   * @param name Instance name for this HSIReadout instance
   */
  explicit HSIReadout(const std::string& name);

  HSIReadout(const HSIReadout&) =
    delete; ///< HSIReadout is not copy-constructible
  HSIReadout& operator=(const HSIReadout&) =
    delete; ///< HSIReadout is not copy-assignable
  HSIReadout(HSIReadout&&) =
    delete; ///< HSIReadout is not move-constructible
  HSIReadout& operator=(HSIReadout&&) =
    delete; ///< HSIReadout is not move-assignable

  void get_info(opmonlib::InfoCollector& ci, int level) override;

private:
  // Commands
  void do_configure(const nlohmann::json& obj) override;


  void read_hsievents(std::atomic<bool>&);
  uint64_t m_readout_counter;
  
  // Configuration
  std::string m_hsi_device_name;
  
  // Source of HSIEvent(s)
  std::string m_connections_file;
  std::unique_ptr<uhal::ConnectionManager> m_connection_manager;
  std::unique_ptr<uhal::HwInterface> m_hsi_device;
  

};
} // namespace timinglibs
} // namespace dunedaq

#endif // TIMINGLIBS_SRC_HSIREADOUT_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

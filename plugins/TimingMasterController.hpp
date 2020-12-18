/**
 * @file TimingMasterController.hpp
 *
 * TimingMasterController is a DAQModule implementation that
 * provides a control interface for timing master hardware.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_PLUGINS_TIMINGMASTERCONTROLLER_HPP_
#define TIMING_PLUGINS_TIMINGMASTERCONTROLLER_HPP_

#include "timing/timingcmd/Structs.hpp"

#include "timing/timingmastercontroller/Structs.hpp"

#include "TimingController.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"

#include <ers/Issue.h>

#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief TimingMasterController is a DAQModule implementation that
 * provides a control interface for timing master hardware.
 */
class TimingMasterController : public dunedaq::timing::TimingController
{
public:
  /**
   * @brief TimingMasterController Constructor
   * @param name Instance name for this TimingMasterController instance
   */
  explicit TimingMasterController(const std::string& name);

  TimingMasterController(const TimingMasterController&) =
    delete; ///< TimingMasterController is not copy-constructible
  TimingMasterController& operator=(const TimingMasterController&) =
    delete; ///< TimingMasterController is not copy-assignable
  TimingMasterController(TimingMasterController&&) =
    delete; ///< TimingMasterController is not move-constructible
  TimingMasterController& operator=(TimingMasterController&&) =
    delete; ///< TimingMasterController is not move-assignable

private:
  // Commands
  void do_configure(const nlohmann::json& obj) override;

  // timing master commands
  void do_masterReset(const nlohmann::json&);
  void do_masterSetTimestamp(const nlohmann::json&);
  void do_masterPrintStatus(const nlohmann::json&);

  // Configuration
  timingmastercontroller::Conf cfg_;

};
} // namespace timing
} // namespace dunedaq

#endif // TIMING_PLUGINS_TIMINGMASTERCONTROLLER_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

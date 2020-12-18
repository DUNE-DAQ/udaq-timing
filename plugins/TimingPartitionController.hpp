/**
 * @file TimingPartitionController.hpp
 *
 * TimingPartitionController is a DAQModule implementation that
 * provides that provides a control interface for a timing partition.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_PLUGINS_TIMINGPARTITIONCONTROLLER_HPP_
#define TIMING_PLUGINS_TIMINGPARTITIONCONTROLLER_HPP_

#include "TimingController.hpp"

#include "timing/timingcmd/Structs.hpp"
#include "timing/timingpartitioncontroller/Structs.hpp"

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
 * @brief TimingPartitionController is a DAQModule implementation that
 * provides that provides a control interface for a timing partition.
 */
class TimingPartitionController : public dunedaq::timing::TimingController
{
public:
  /**
   * @brief TimingPartitionController Constructor
   * @param name Instance name for this TimingPartitionController instance
   */
  explicit TimingPartitionController(const std::string& name);

  TimingPartitionController(const TimingPartitionController&) =
    delete; ///< TimingPartitionController is not copy-constructible
  TimingPartitionController& operator=(const TimingPartitionController&) =
    delete; ///< TimingPartitionController is not copy-assignable
  TimingPartitionController(TimingPartitionController&&) =
    delete; ///< TimingPartitionController is not move-constructible
  TimingPartitionController& operator=(TimingPartitionController&&) =
    delete; ///< TimingPartitionController is not move-assignable

private:
  // Commands
  void do_configure(const nlohmann::json& obj) override;

  // timing partition commands
  void do_partitionConfigure(const nlohmann::json&);
  void do_partitionEnable(const nlohmann::json&);
  void do_partitionDisable(const nlohmann::json&);
  void do_partitionStart(const nlohmann::json&);
  void do_partitionStop(const nlohmann::json&);
  void do_partitionEnableTriggers(const nlohmann::json&);
  void do_partitionDisableTriggers(const nlohmann::json&);
  void do_partitionPrintStatus(const nlohmann::json&);

  // Configuration
  timingpartitioncontroller::Conf cfg_;

};
} // namespace timing
} // namespace dunedaq

#endif // TIMING_PLUGINS_TIMINGPARTITIONCONTROLLER_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

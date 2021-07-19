/**
 * @file TimingFanoutController.hpp
 *
 * TimingFanoutController is a DAQModule implementation that
 * provides a control interface for timing master hardware.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMINGLIBS_PLUGINS_TIMINGFANOUTCONTROLLER_HPP_
#define TIMINGLIBS_PLUGINS_TIMINGFANOUTCONTROLLER_HPP_

#include "timinglibs/timingcmd/Nljs.hpp"
#include "timinglibs/timingcmd/Structs.hpp"

#include "timinglibs/timingfanoutcontroller/Nljs.hpp"
#include "timinglibs/timingfanoutcontroller/Structs.hpp"

#include "timinglibs/timingfanoutcontrollerinfo/InfoNljs.hpp"
#include "timinglibs/timingfanoutcontrollerinfo/InfoStructs.hpp"

#include "TimingController.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"

#include "ers/Issue.hpp"
#include "logging/Logging.hpp"

#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace timinglibs {

/**
 * @brief TimingFanoutController is a DAQModule implementation that
 * provides a control interface for timing master hardware.
 */
class TimingFanoutController : public dunedaq::timinglibs::TimingController
{
public:
  /**
   * @brief TimingFanoutController Constructor
   * @param name Instance name for this TimingFanoutController instance
   */
  explicit TimingFanoutController(const std::string& name);

  TimingFanoutController(const TimingFanoutController&) = delete; ///< TimingFanoutController is not copy-constructible
  TimingFanoutController& operator=(const TimingFanoutController&) =
    delete;                                                  ///< TimingFanoutController is not copy-assignable
  TimingFanoutController(TimingFanoutController&&) = delete; ///< TimingFanoutController is not move-constructible
  TimingFanoutController& operator=(TimingFanoutController&&) =
    delete; ///< TimingFanoutController is not move-assignable

  void init(const nlohmann::json& init_data) override;
private:
  // Commands
  void do_configure(const nlohmann::json& obj) override;

  void construct_fanout_hw_cmd(timingcmd::TimingHwCmd& hw_cmd, const std::string& cmd_id);

  // timing master commands
  void do_fanout_io_reset(const nlohmann::json& data);
  void do_fanout_print_status(const nlohmann::json&);

  // pass op mon info
  void get_info(opmonlib::InfoCollector& ci, int level) override;
};
} // namespace timinglibs
} // namespace dunedaq

#endif // TIMINGLIBS_PLUGINS_TIMINGFANOUTCONTROLLER_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

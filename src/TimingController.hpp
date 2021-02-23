/**
 * @file TimingController.hpp
 *
 * TimingController is a DAQModule implementation that
 * provides a base class for timing controller modules.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_SRC_TIMINGCONTROLLER_HPP_
#define TIMING_SRC_TIMINGCONTROLLER_HPP_

#include "timing/timingcmd/Structs.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"

#include "appfwk/app/Structs.hpp"
#include "appfwk/app/Nljs.hpp"

#include <ers/Issue.h>

#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief TimingController is a DAQModule implementation that
 * provides a base class for timing controller modules.
 */
class TimingController : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief TimingController Constructor
   * @param name Instance name for this TimingController instance
   */
  explicit TimingController(const std::string& name);

  TimingController(const TimingController&) =
    delete; ///< TimingController is not copy-constructible
  TimingController& operator=(const TimingController&) =
    delete; ///< TimingController is not copy-assignable
  TimingController(TimingController&&) =
    delete; ///< TimingController is not move-constructible
  TimingController& operator=(TimingController&&) =
    delete; ///< TimingController is not move-assignable

  void init( const nlohmann::json& init_data) override;

protected:
  // Commands
  virtual void do_configure(const nlohmann::json& obj) = 0;
  virtual void do_start(const nlohmann::json& obj);
  virtual void do_stop(const nlohmann::json& obj);

  // Configuration
  using sink_t = dunedaq::appfwk::DAQSink<timingcmd::TimingHwCmd>;
  std::unique_ptr<sink_t> m_hw_command_out_queue;
  std::chrono::milliseconds m_hw_cmd_out_queue_timeout;

  virtual void send_hw_cmd(const timingcmd::TimingHwCmd& hw_cmd);

};
} // namespace timing
} // namespace dunedaq

#endif // TIMING_SRC_TIMINGCONTROLLER_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

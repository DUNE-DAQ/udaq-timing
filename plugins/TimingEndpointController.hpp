/**
 * @file TimingEndpointController.hpp
 *
 * TimingEndpointController is a DAQModule implementation that
 * provides that provides a control interface for a timing endpoint.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_PLUGINS_TIMINGENDPOINTCONTROLLER_HPP_
#define TIMING_PLUGINS_TIMINGENDPOINTCONTROLLER_HPP_

#include "TimingController.hpp"

#include "timing/timingcmd/Structs.hpp"
#include "timing/timingendpointcontroller/Structs.hpp"

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
 * @brief TimingEndpointController is a DAQModule implementation that
 * provides that provides a control interface for a timing endpoint.
 */
class TimingEndpointController : public dunedaq::timing::TimingController
{
public:
  /**
   * @brief TimingEndpointController Constructor
   * @param name Instance name for this TimingEndpointController instance
   */
  explicit TimingEndpointController(const std::string& name);

  TimingEndpointController(const TimingEndpointController&) =
    delete; ///< TimingEndpointController is not copy-constructible
  TimingEndpointController& operator=(const TimingEndpointController&) =
    delete; ///< TimingEndpointController is not copy-assignable
  TimingEndpointController(TimingEndpointController&&) =
    delete; ///< TimingEndpointController is not move-constructible
  TimingEndpointController& operator=(TimingEndpointController&&) =
    delete; ///< TimingEndpointController is not move-assignable

private:
  // Commands
  void do_configure(const nlohmann::json& obj) override;

  // timing endpoint commands
  void do_endpoint_io_reset(const nlohmann::json&);
  void do_endpoint_enable(const nlohmann::json&);
  void do_endpoint_disable(const nlohmann::json&);
  void do_endpoint_reset(const nlohmann::json&);
  void do_endpoint_print_status(const nlohmann::json&);
  void do_endpoint_print_timestamp(const nlohmann::json&);

  // Configuration
  timingendpointcontroller::Conf m_cfg;

};
} // namespace timing
} // namespace dunedaq

#endif // TIMING_PLUGINS_TIMINGENDPOINTCONTROLLER_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

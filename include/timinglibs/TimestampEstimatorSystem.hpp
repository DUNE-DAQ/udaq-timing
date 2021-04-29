/**
 * @file TimestampEstimatorSystem.hpp TimestampEstimatorSystem Class
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMINGLIBS_INCLUDE_TIMINGLIBS_TIMESTAMPESTIMATORSYSTEM_HPP_
#define TIMINGLIBS_INCLUDE_TIMINGLIBS_TIMESTAMPESTIMATORSYSTEM_HPP_

#include "timinglibs/TimestampEstimatorBase.hpp"
#include "timinglibs/TimingIssues.hpp"

namespace dunedaq {
namespace timinglibs {

/**
 * @brief TimestampEstimatorSystem is an implementation of
 * TimestampEstimatorBase that uses the system clock to give the current timestamp
 **/
class TimestampEstimatorSystem : public TimestampEstimatorBase
{
public:
  TimestampEstimatorSystem(uint64_t clock_frequency_hz);

  dfmessages::timestamp_t get_timestamp_estimate() const override;

private:
  uint64_t m_clock_frequency_hz; // NOLINT
};

} // namespace timinglibs
} // namespace dunedaq

#endif // TIMINGLIBS_INCLUDE_TIMINGLIBS_TIMESTAMPESTIMATORSYSTEM_HPP_
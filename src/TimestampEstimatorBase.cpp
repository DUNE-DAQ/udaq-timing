/**
 * @file TimestampEstimatorBase.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timinglibs/TimestampEstimatorBase.hpp"

#include <thread>

namespace dunedaq {
namespace timinglibs {
TimestampEstimatorBase::WaitStatus
TimestampEstimatorBase::wait_for_valid_timestamp(std::atomic<bool>& continue_flag)
{
  if (!continue_flag.load())
    return TimestampEstimatorBase::kInterrupted;

  while (continue_flag.load() && get_timestamp_estimate() == dfmessages::TypeDefaults::s_invalid_timestamp) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    if (!continue_flag.load())
      return TimestampEstimatorBase::kInterrupted;
  }

  return TimestampEstimatorBase::kFinished;
}

TimestampEstimatorBase::WaitStatus
TimestampEstimatorBase::wait_for_timestamp(dfmessages::timestamp_t ts, std::atomic<bool>& continue_flag)
{
  if (!continue_flag.load())
    return TimestampEstimatorBase::kInterrupted;

  while (continue_flag.load() &&
         (get_timestamp_estimate() < ts || get_timestamp_estimate() == dfmessages::TypeDefaults::s_invalid_timestamp)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    if (!continue_flag.load())
      return TimestampEstimatorBase::kInterrupted;
  }

  return TimestampEstimatorBase::kFinished;
}

} // namespace timinglibs
} // namespace dunedaq

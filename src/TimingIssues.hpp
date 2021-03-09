/**
 * @file TimingIssues.hpp
 *
 * This file contains the definitions of ERS Issues that are common
 * to two or more of the DAQModules in this package.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMINGLIBS_SRC_TIMINGISSUES_HPP_
#define TIMINGLIBS_SRC_TIMINGISSUES_HPP_

#include "appfwk/DAQModule.hpp"
#include "ers/Issue.hpp"

#include <string>

namespace dunedaq {

ERS_DECLARE_ISSUE_BASE(timinglibs,
                       ProgressUpdate,
                       appfwk::GeneralDAQModuleIssue,
                       message,
                       ((std::string)name),
                       ((std::string)message))

ERS_DECLARE_ISSUE_BASE(timinglibs,
                       InvalidQueueFatalError,
                       appfwk::GeneralDAQModuleIssue,
                       "The " << queueType << " queue was not successfully created.",
                       ((std::string)name),
                       ((std::string)queueType))

ERS_DECLARE_ISSUE(timinglibs,                         ///< Namespace
                  UHALIssue,                          ///< Issue class name
                  " UHAL related issue: " << message, ///< Message
                  ((std::string)message)              ///< Message parameters
)

ERS_DECLARE_ISSUE_BASE(timinglibs,
                       UHALConnectionsFileIssue,
                       timinglibs::UHALIssue,
                       " UHAL connections file issue: " << message,
                       ((std::string)message),
                       ERS_EMPTY
)

ERS_DECLARE_ISSUE_BASE(timinglibs,
                       InvalidUHALLogLevel,
                       timinglibs::UHALIssue,
                       " Invalid UHAL log level supplied: " << log_level,
                       ((std::string)log_level),
                       ERS_EMPTY
)

ERS_DECLARE_ISSUE_BASE(timinglibs,
                       UHALDeviceNameIssue,
                       timinglibs::UHALIssue,
                       " UHAL device name issue: " << message,
                       ((std::string)message),
                       ERS_EMPTY
)

ERS_DECLARE_ISSUE(timinglibs,
                  FailedToCollectOpMonInfo,
                  " Failed to collect op mon info class: " << info_class << " from device: " << device_name,
                  ((std::string)info_class)((std::string)device_name)
                  )
} // namespace dunedaq

#endif // TIMINGLIBS_SRC_TIMINGISSUES_HPP_

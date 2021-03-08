/**
 * @file TimingHardwareManagerPDI.hpp
 *
 * TimingHardwareManagerPDI is a DAQModule implementation that
 * provides the interface to the timing system hardware.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMINGLIBS_PLUGINS_TIMINGHARDWAREMANAGERPDI_HPP_
#define TIMINGLIBS_PLUGINS_TIMINGHARDWAREMANAGERPDI_HPP_

#include "timinglibs/timingcmd/Structs.hpp"
#include "timinglibs/timingcmd/Nljs.hpp"

#include "timinglibs/timinghardwaremanagerpdi/Structs.hpp"
#include "timinglibs/timinghardwaremanagerpdi/Nljs.hpp"

#include "timinglibs/timinghardwaremanagerpdiinfo/Structs.hpp"
#include "timinglibs/timinghardwaremanagerpdiinfo/Nljs.hpp"

#include "TimingHardwareManager.hpp"
#include "InfoGatherer.hpp"

#include "TimingIssues.hpp"

// in timing-board-software at the moment
#include "timing/timingfirmwareinfo/Structs.hpp"
#include "timing/timingfirmwareinfo/Nljs.hpp"

#include "timing/PDIMasterDesign.hpp"
#include "timing/EndpointDesign.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQModuleHelper.hpp"

#include "appfwk/app/Structs.hpp"
#include "appfwk/app/Nljs.hpp"

#include "ers/Issue.hpp"
#include "logging/Logging.hpp"

#include "uhal/ConnectionManager.hpp"
#include "uhal/utilities/files.hpp"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <regex>

namespace dunedaq {
namespace timinglibs {

/**
 * @brief Hardware manager for PD-I hardware.
 */
class TimingHardwareManagerPDI : public TimingHardwareManager<timing::PDIMasterDesign<timing::TLUIONode>,timing::EndpointDesign<timing::FMCIONode>>
{
public:
  /**
   * @brief TimingHardwareManagerPDI Constructor
   * @param name Instance name for this TimingHardwareManagerPDI instance
   */
  explicit TimingHardwareManagerPDI(const std::string& name);

  TimingHardwareManagerPDI(const TimingHardwareManagerPDI&) =
    delete; ///< TimingHardwareManagerPDI is not copy-constructible
  TimingHardwareManagerPDI& operator=(const TimingHardwareManagerPDI&) =
    delete; ///< TimingHardwareManagerPDI is not copy-assignable
  TimingHardwareManagerPDI(TimingHardwareManagerPDI&&) =
    delete; ///< TimingHardwareManagerPDI is not move-constructible
  TimingHardwareManagerPDI& operator=(TimingHardwareManagerPDI&&) =
    delete; ///< TimingHardwareManagerPDI is not move-assignable

  // configuration
private:
  timinghardwaremanagerpdi::ConfParams m_cfg;
  void do_configure(const data_t& obj) override;
  void do_start(const nlohmann::json&) override;
  void do_stop(const nlohmann::json&)  override;

  // monitoring
  InfoGatherer<timing::timingfirmwareinfo::TimingPDIMasterTLUMonitorData> m_master_monitor_data_gatherer;
  virtual void gather_master_monitor_data(InfoGatherer<timing::timingfirmwareinfo::TimingPDIMasterTLUMonitorData>& gatherer);

  InfoGatherer<timing::timingfirmwareinfo::TimingEndpointFMCMonitorData> m_endpoint_monitor_data_gatherer;
  virtual void gather_endpoint_monitor_data(InfoGatherer<timing::timingfirmwareinfo::TimingEndpointFMCMonitorData>& gatherer);

  InfoGatherer<timing::timingfirmwareinfo::TimingPDIMasterTLUMonitorDataDebug> m_master_monitor_data_gatherer_debug;
  virtual void gather_master_monitor_data_debug(InfoGatherer<timing::timingfirmwareinfo::TimingPDIMasterTLUMonitorDataDebug>& gatherer);

  InfoGatherer<timing::timingfirmwareinfo::TimingEndpointFMCMonitorDataDebug> m_endpoint_monitor_data_gatherer_debug;
  virtual void gather_endpoint_monitor_data_debug(InfoGatherer<timing::timingfirmwareinfo::TimingEndpointFMCMonitorDataDebug>& gatherer);


  void get_info(opmonlib::InfoCollector & ci, int level) override;
};
} // namespace timinglibs
} // namespace dunedaq

#endif // TIMINGLIBS_PLUGINS_TIMINGHARDWAREMANAGERPDI_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

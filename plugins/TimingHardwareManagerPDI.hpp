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

#ifndef TIMING_PLUGINS_TIMINGHARDWAREMANAGERPDI_HPP_
#define TIMING_PLUGINS_TIMINGHARDWAREMANAGERPDI_HPP_

#include "timing/timingcmd/Structs.hpp"
#include "timing/timingcmd/Nljs.hpp"

#include "timing/timinghardwaremanagerpdi/Structs.hpp"
#include "timing/timinghardwaremanagerpdi/Nljs.hpp"

#include "TimingHardwareManager.hpp"

#include "CommonIssues.hpp"

// in timing-board-software at the moment
#include "pdt/timingmon/Structs.hpp"
#include "pdt/PDIMasterDesign.hpp"
#include "pdt/EndpointDesign.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQModuleHelper.hpp"

#include "ers/Issue.h"

#include "uhal/ConnectionManager.hpp"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <regex>

namespace dunedaq {
namespace timing {

/**
 * @brief Hardware manager for PD-I hardware.
 */
class TimingHardwareManagerPDI : public TimingHardwareManager<pdt::PDIMasterDesign<pdt::TLUIONode>,pdt::EndpointDesign<pdt::FMCIONode>>
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
  timinghardwaremanagerpdi::Conf m_cfg;
  void do_configure(const data_t& obj) override;
  void do_start(const nlohmann::json&) override;
  void do_stop(const nlohmann::json&)  override;

  // monitoring
  ModuleMonitor<pdt::timingmon::TimingPDIMasterDesignTLUMonitorData> m_monitor_data_gatherer;
  virtual void gather_monitor_data(std::atomic<bool>& monitor_running, ModuleMonitor<pdt::timingmon::TimingPDIMasterDesignTLUMonitorData>& monitor, std::atomic<uint>& monitor_interval);

  void get_info(const nlohmann::json&) override;
};
} // namespace timing
} // namespace dunedaq

#endif // TIMING_PLUGINS_TIMINGHARDWAREMANAGERPDI_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

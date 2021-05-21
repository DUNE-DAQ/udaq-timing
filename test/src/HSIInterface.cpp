/**
 * @file HSIInterface.cpp HSIInterface class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "HSIInterface.hpp"

#include "timinglibs/TimingIssues.hpp"

#include "appfwk/app/Nljs.hpp"

#include "logging/Logging.hpp"

#include <chrono>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

namespace dunedaq {
namespace timinglibs {

HSIInterface::HSIInterface(const std::string& name, std::function<void(std::atomic<bool>&)> do_work)
  : dunedaq::appfwk::DAQModule(name)
  , m_thread(do_work)
  , m_hsievent_sink(nullptr)
  , m_queue_timeout(1)
  , m_sent_counter(0)
  , m_failed_to_send_counter(0)
  , m_last_sent_timestamp(0)
{
  // register_command("conf",  &HSIInterface::do_configure);
  // register_command("start", &HSIInterface::do_start);
  // register_command("stop",  &HSIInterface::do_stop);
  // register_command("scrap", &HSIInterface::do_scrap);
}

void
HSIInterface::init(const nlohmann::json& init_data)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";

  m_hsievent_sink.reset(new appfwk::DAQSink<dfmessages::HSIEvent>(appfwk::queue_inst(init_data, "hsievent_sink")));

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

void
HSIInterface::get_info(opmonlib::InfoCollector& /*ci*/, int /*level*/)
{}

void
HSIInterface::do_start(const nlohmann::json& /*args*/)
{
  TLOG() << get_name() << ": Entering do_start() method";
  m_thread.start_working_thread("fake-tsd-gen");
  TLOG() << get_name() << " successfully started";
  TLOG() << get_name() << ": Exiting do_start() method";
}

void
HSIInterface::do_stop(const nlohmann::json& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
  m_thread.stop_working_thread();
  TLOG() << get_name() << " successfully stopped";
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
}

void
HSIInterface::do_scrap(const nlohmann::json& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_scrap() method";
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_scrap() method";
}

void
HSIInterface::send_hsi_event(dfmessages::HSIEvent& event)
{
  TLOG_DEBUG(3) << get_name() << ": Sending HSIEvent: " << event.header << ", " << std::bitset<32>(event.signal_map)
                << ", " << event.timestamp << ", " << event.sequence_counter << "\n";

  std::string thisQueueName = m_hsievent_sink->get_name();

  TLOG_DEBUG(3) << get_name() << ": Pushing the generated HSIEvent onto queue " << thisQueueName;
  bool was_successfully_sent = false;
  while (!was_successfully_sent) {
    try {
      m_hsievent_sink->push(event, m_queue_timeout);
      ++m_sent_counter;
      m_last_sent_timestamp.store(event.timestamp);
      was_successfully_sent = true;
    } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
      std::ostringstream oss_warn;
      oss_warn << "push to output queue \"" << thisQueueName << "\"";
      ers::error(dunedaq::appfwk::QueueTimeoutExpired(ERS_HERE, get_name(), oss_warn.str(), m_queue_timeout.count()));
      ++m_failed_to_send_counter;
    }
  }
  if (m_sent_counter > 0 && m_sent_counter % 200000 == 0)
    TLOG_DEBUG(3) << "Have sent out " << m_sent_counter << " HSI events";
}

} // namespace timinglibs
} // namespace dunedaq

// Local Variables:
// c-basic-offset: 2
// End:

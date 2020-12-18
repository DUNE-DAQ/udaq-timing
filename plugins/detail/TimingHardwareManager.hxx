namespace dunedaq::udaqtiming {

template<typename Child>
void
TimingHardwareManager::register_timing_hw_command(const std::string& name, void (Child::*f)(const timingcmd::TimingCmd&))
{
  using namespace std::placeholders;

  bool done = timingHwCmdMap_.emplace(name, std::bind(f, dynamic_cast<Child*>(this), _1)).second;
  if (!done) {
  	// TODO throw specific error
  	throw dunedaq::appfwk::CommandRegistrationFailed(ERS_HERE, get_name(), name);
  }
}

} // namespace dunedaq::udaqtiming
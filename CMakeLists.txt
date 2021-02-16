cmake_minimum_required(VERSION 3.12)
project(timing VERSION 0.0.1)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

# ers: for exceptions and normal output
# TRACE: for low-level debugging statements

find_package(ers REQUIRED)     
find_package(TRACE REQUIRED)
find_package(appfwk REQUIRED)
find_package(timing-board-software REQUIRED)

##############################################################################
daq_add_library(TimingController.cpp LINK_LIBRARIES ers::ers appfwk::appfwk)

##############################################################################
daq_add_plugin(TimingHardwareManagerPDI duneDAQModule SCHEMA LINK_LIBRARIES ers::ers appfwk::appfwk timing-board-software libatomic.so)
daq_add_plugin(TimingMasterController duneDAQModule SCHEMA LINK_LIBRARIES ers::ers appfwk::appfwk timing)
daq_add_plugin(TimingPartitionController duneDAQModule SCHEMA LINK_LIBRARIES ers::ers appfwk::appfwk timing)
daq_add_plugin(TimingEndpointController duneDAQModule SCHEMA LINK_LIBRARIES ers::ers appfwk::appfwk timing)


##############################################################################
target_include_directories(${PROJECT_NAME}_TimingHardwareManagerPDI_duneDAQModule PUBLIC $ENV{UHAL_INC})

##############################################################################
daq_install()

cmake_minimum_required(VERSION 3.12)
project(udaqtiming VERSION 0.0.1)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

# ers: for exceptions and normal output
# TRACE: for low-level debugging statements

find_package(ers REQUIRED)     
find_package(TRACE REQUIRED)
find_package(appfwk REQUIRED)

##############################################################################
set(LIST_OF_TIMING_SOFTWARE_LIBS $ENV{TIMING_BOARD_SOFTWARE_LIB}/libprotodune_pdt_core.so ) # list of timing board software libs.

##############################################################################
daq_add_library(TimingController.cpp LINK_LIBRARIES ers::ers appfwk::appfwk)

##############################################################################
daq_add_plugin(TimingHardwareManager duneDAQModule SCHEMA LINK_LIBRARIES ers::ers appfwk::appfwk ${LIST_OF_TIMING_SOFTWARE_LIBS})
daq_add_plugin(TimingMasterController duneDAQModule SCHEMA LINK_LIBRARIES ers::ers appfwk::appfwk udaqtiming)
daq_add_plugin(TimingPartitionController duneDAQModule SCHEMA LINK_LIBRARIES ers::ers appfwk::appfwk udaqtiming)


##############################################################################
target_include_directories(${PROJECT_NAME}_TimingHardwareManager_duneDAQModule PUBLIC $ENV{TIMING_BOARD_SOFTWARE_INC} $ENV{UHAL_INC})

##############################################################################
daq_install()

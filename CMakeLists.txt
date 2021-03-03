cmake_minimum_required(VERSION 3.12)
project(timing VERSION 0.0.1)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(ers REQUIRED)     
find_package(logging REQUIRED)
find_package(appfwk REQUIRED)
find_package(timing-board-software REQUIRED)
find_package(nlohmann_json REQUIRED)

daq_codegen(*.jsonnet TEMPLATES Structs.hpp.j2 Nljs.hpp.j2 )

##############################################################################
# Dependency sets
set(TIMING_DEPENDENCIES 
  appfwk::appfwk
  ers::ers
  logging::logging
  opmonlib::opmonlib
  nlohmann_json::nlohmann_json
)

##############################################################################
daq_add_library(TimingController.cpp LINK_LIBRARIES ${TIMING_DEPENDENCIES})

##############################################################################
daq_add_plugin(TimingHardwareManagerPDI duneDAQModule LINK_LIBRARIES timing-board-software timing)
daq_add_plugin(TimingMasterController duneDAQModule LINK_LIBRARIES timing)
daq_add_plugin(TimingPartitionController duneDAQModule LINK_LIBRARIES timing)
daq_add_plugin(TimingEndpointController duneDAQModule LINK_LIBRARIES timing)


##############################################################################
target_include_directories(${PROJECT_NAME}_TimingHardwareManagerPDI_duneDAQModule PUBLIC $ENV{UHAL_INC} $ENV{PUGIXML_INC})

##############################################################################
daq_install()
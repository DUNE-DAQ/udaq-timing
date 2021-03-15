cmake_minimum_required(VERSION 3.12)
project(timinglibs VERSION 1.0.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(ers REQUIRED)     
find_package(logging REQUIRED)
find_package(appfwk REQUIRED)
find_package(timing REQUIRED)
find_package(nlohmann_json REQUIRED)

daq_codegen(*.jsonnet TEMPLATES Structs.hpp.j2 Nljs.hpp.j2 )

##############################################################################
# Dependency sets
set(TIMINGLIBS_DEPENDENCIES 
  appfwk::appfwk
  ers::ers
  logging::logging
  opmonlib::opmonlib
  nlohmann_json::nlohmann_json
)

##############################################################################
daq_add_library(TimingController.cpp LINK_LIBRARIES ${TIMINGLIBS_DEPENDENCIES})

##############################################################################
daq_add_plugin(TimingHardwareManagerPDI duneDAQModule LINK_LIBRARIES timing timinglibs)
daq_add_plugin(TimingMasterController duneDAQModule LINK_LIBRARIES timinglibs)
daq_add_plugin(TimingPartitionController duneDAQModule LINK_LIBRARIES timinglibs)
daq_add_plugin(TimingEndpointController duneDAQModule LINK_LIBRARIES timinglibs)


##############################################################################
target_include_directories(${PROJECT_NAME}_TimingHardwareManagerPDI_duneDAQModule PUBLIC $ENV{UHAL_INC} $ENV{PUGIXML_INC})

##############################################################################
daq_install()

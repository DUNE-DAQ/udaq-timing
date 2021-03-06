cmake_minimum_required(VERSION 3.12)
project(timinglibs VERSION 1.1.1)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(ers REQUIRED)     
find_package(logging REQUIRED)
find_package(appfwk REQUIRED)
find_package(timing REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(dfmessages REQUIRED)
find_package(opmonlib REQUIRED)

daq_codegen( fakehsieventgenerator.jsonnet
			 hsicontroller.jsonnet
			 hsireadout.jsonnet
			 timingcmd.jsonnet
			 timingendpointcontroller.jsonnet
			 timinghardwaremanagerpdi.jsonnet
			 timingmastercontroller.jsonnet
			 timingpartitioncontroller.jsonnet
			 timingfanoutcontroller.jsonnet 
			 TEMPLATES Structs.hpp.j2 Nljs.hpp.j2 )

daq_codegen( fakehsieventgeneratorinfo.jsonnet 
			 hsicontrollerinfo.jsonnet 
			 hsireadoutinfo.jsonnet 
			 timingendpointcontrollerinfo.jsonnet 
			 timinghardwaremanagerpdiinfo.jsonnet  
			 timingmastercontrollerinfo.jsonnet
			 timingpartitioncontrollerinfo.jsonnet
			 timingfanoutcontrollerinfo.jsonnet
			 DEP_PKGS opmonlib TEMPLATES opmonlib/InfoStructs.hpp.j2 opmonlib/InfoNljs.hpp.j2 )

##############################################################################
# Dependency sets
set(TIMINGLIBS_DEPENDENCIES 
  appfwk::appfwk
  ers::ers
  logging::logging
  opmonlib::opmonlib
  nlohmann_json::nlohmann_json
  dfmessages::dfmessages
  timing::timing
)

##############################################################################
daq_add_library(TimingController.cpp TimestampEstimatorBase.cpp TimestampEstimator.cpp TimestampEstimatorSystem.cpp LINK_LIBRARIES ${TIMINGLIBS_DEPENDENCIES})
target_include_directories(${PROJECT_NAME} PUBLIC $ENV{UHAL_INC} $ENV{PUGIXML_INC})


##############################################################################
daq_add_plugin(TimingHardwareManagerPDI duneDAQModule LINK_LIBRARIES timing::timing timinglibs)
daq_add_plugin(TimingMasterController duneDAQModule LINK_LIBRARIES timinglibs)
daq_add_plugin(TimingFanoutController duneDAQModule LINK_LIBRARIES timinglibs)
daq_add_plugin(TimingPartitionController duneDAQModule LINK_LIBRARIES timinglibs)
daq_add_plugin(TimingEndpointController duneDAQModule LINK_LIBRARIES timinglibs)
daq_add_plugin(FakeHSIEventGenerator duneDAQModule LINK_LIBRARIES timinglibs)
daq_add_plugin(HSIReadout duneDAQModule LINK_LIBRARIES timing::timing timinglibs)
daq_add_plugin(HSIController duneDAQModule LINK_LIBRARIES timing::timing timinglibs)

##############################################################################
target_include_directories(${PROJECT_NAME}_TimingHardwareManagerPDI_duneDAQModule PUBLIC $ENV{UHAL_INC} $ENV{PUGIXML_INC})

##############################################################################
daq_add_unit_test(TimestampEstimatorSystem_test  LINK_LIBRARIES timinglibs)
daq_add_unit_test(TimestampEstimator_test        LINK_LIBRARIES timinglibs)

##############################################################################
daq_install()

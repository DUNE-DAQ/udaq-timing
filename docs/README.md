
# timinglibs:"The DUNE DAQ timing application"
## Overview
#### In a single sentence
timinglibs is a repository containing a collection of timing DAQModules, which together form the timing DAQ application.

#### In a bit more detail:

## Running timinglibs

Instructions on building and running the timing application can be found on the repository wiki: https://github.com/DUNE-DAQ/timinglibs/wiki.

The `timinglibs/python/timinglibs/timing_app_confgen.py` script generates a json configuration file containing the commands to be received by the "timing" DUNE DAQ modules.

#### Script arguments and options

The script takes in one argument which is the file name of the produced json file. The default file name is `timing_app.json`. The script is also able to accept the following command line options:

* `-r` or `--run-number`
   
   Run number parameter for the `start` `rc` command. Not used in any particular way by the current "timing" modules. Default: `333`.

* `-g` or `--gather-interval`
   
   Period (in us) between queries to timing firmware+hardware for "essential" (i.e. level 1) operational monitoring information. Default: `1e6` (us).

* `-d` or `--gather-interval-debug`

   Period (in us) between queries to timing firmware+hardware for "debug" (i.e. level > 1) operational monitoring information. Default: `10e6` (us). 
   
   N.B. This querying involves I2C transactions, setting too short a period may lead to software instability.
    
* `-m` or `--master-device-name`

   Device name of the timing master (TLU) to be monitored and controlled by the timing application. Default: `PROD_MASTER`.

* `-e` or `--endpoint-device-name`

   Device name of an FMC based test endpoint to be monitored and controlled by the timing application. Default: `` (i.e. empty, no endpoint present to be monitored or controlled).
    
* `-u` or `--uhal-log-level`

   String to control the uhal logging level. Possible values are: `fatal, error, warning, notice, info, debug`. Default: `notice`.

* `-o` or `--output-path`

   Path of the output json file. Default: `.` (current directory).

## DUNE DAQ modules
The `init` and `conf` commands generated with above script instantiate and configure the following DUNE DAQ modules.

* `thi`

   This is an instance of the `TimingHardwareManagerPDI` class, it receives hardware commands from the timing controller modules, and makes the appropriate calls to the hardware over IPBus. It is also responsible for extracting operational monitoring information from the timing master and the test FMC-based endpoint. The names of the timing master and endpoint devices to be monitored are set using the `--master-device-name` and `--endpoint-device-name` options described above.

* `tmc0`

  This is an instance of a `TimingMasterController` module, it receives commands from an external source, e.g. a timing system operator or run control, and translates those commands to timing hardware commands which are then sent to the `thi` module. The master hardware commands issued by this module are addressed to the device specified via the `--master-device-name` option.

* `tpc0`

   This is an instance of a `TimingPartitionController` module, it receives timing partition commands from an external source, e.g. a timing system operator or run control, and translates those commands to timing hardware commands which are then sent to the `thi` module. The partition hardware commands issued by this module are addressed partition `0` on the device specified via the `--master-device-name` option.

* `tec0`

   This is an instance of a `TimingEndpointController` module, it receives timing endpoint commands from an external source, e.g. a timing system operator or run control, and translates those commands to timing hardware commands which are then sent to the `thi` module. The endpoint hardware commands issued by this module are addressed endpoint `0` on the device specified via the `--endpoint-device-name` option.

## The timinglib functions: plugins

timinglibs contains several modular functions which are described below:


#### TimingMasterController

#### TimingPartitionController

#### TimingHardwareManager

#### TimingEndpointController

#### HSIController

#### HSIReadout

#### FakeHSIEventGenerator



   
   








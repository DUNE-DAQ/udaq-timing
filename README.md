# timing
Timing DAQModules.

## Building
1. Prepare your work area as per instructions given here: https://github.com/DUNE-DAQ/appfwk/wiki/Compiling-and-running-under-v2.0.0.
2. Add the product directory, `/cvmfs/dune.opensciencegrid.org/dunedaq/DUNE/products_dev`, to your `dune_products_dirs` list in your `.dunedaq_area` file.
3. Add the following products to your `dune_products` list in your `.dunedaq_area` file.
```
"uhal v2_6_4 e19:prof"
"timing_board_software v6_0_0b1 e19:prof"
```
4. Clone this repository into the `sourcecode` directory of your work area.
5. Run the command `setup_build_environment`.
6. `cd` into `timing/schema`, and run `generate.sh`. This creates the header files containing the timingcmd structures.
7. To build the timing package, run the command, `build_daq_software.sh --install`.

## Running
1. If not already sourced, source `daq-buildtools/setup_dbt.sh`.
2. Configure the DUNE DAQ run environment by running the command `setup_runtime_environment`.
3. The uhal connnections file in `timing-board-software` makes use of the environment variable `PDT_TESTS` to find the relevant IPBus address maps. The `timing-board-software` core library uses the same environment variable to locate PLL configuration files. `PDT_TESTS` is also used to construct the connections file path specified in `timing/schema/timingtiming_job.jsonnet`. Set `PDT_TESTS` to point to your instance of the directory, `timing-board-software/tests/`.
4. Clone a copy of the DUNE DAQ `appfwk` repository, https://github.com/DUNE-DAQ/appfwk. To make the instructions in the next step "copy and paste" compatible, clone the repository in the directory which contains the `daq-buildtools` directory.
5. The file `timing/schema/timing_job.jsonnet` describes the timing commands available to be injected via the DUNE DAQ `restcmd` interface. It also contains the configuration for the timing DUNE DAQ modules. Ensure the device names to be controlled by the timing DAQ modules are the correct ones. Ensure the connection file path specified is valid. To translate the `timing_job.jsonnet` file into a JSON file, navigate to `timning/schema` and execute the following command:
```
moo -M ../../../../appfwk/schema compile timing_job.jsonnet > timing_job.json
```
where `../../../../appfwk/schema`, is the schema directory of your instance of the DUNE DAQ `appfwk` repository.

6. Start an instance of the DUNE DAQ application, with a `restcmd` interface listening on the localhost port 12345, by executing the command
```
daq_application --commandFacility rest://localhost:12345
```
7. Start another session on the machine on which the DUNE DAQ application is running, and source `daq-buildtools/setup_dbt.sh`.
8. Navigate to your work area, and execute the commands below.
```
setup_runtime_environment
curl -O https://raw.githubusercontent.com/DUNE-DAQ/restcmd/dunedaq-v2.0.0/scripts/send-restcmd.py
```
9. To start the transmit end of the `restcmd` inteface, execute the command
```
python ./send-restcmd.py --interactive --file ./sourcecode/timing/schema/timing_job.json
```
At this point you should see the list of available commands, e.g. `init`, `start`, `conf`, `master_reset`.

10. To send a command, type the command you wish to send and press return. The sequence of commands to configure the timing DAQModules, and the reset the master hardware (TLU) is itemised below.
```
init
conf
start
master_reset
```
After the sending of each command, you should see the DUNE DAQ application responding via the session running the application.
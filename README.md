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
6. `cd` into `timing/schema`, and run `generate.sh`. This creates the timing DAQModules configuration header files.
7. To build the timing package, run the command, `build_daq_software.sh --install`.

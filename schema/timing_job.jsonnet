local moo = import "moo.jsonnet";

local cmd = import "appfwk-cmd-make.jsonnet";
local thi = import "udaqtiming-TimingHardwareManager-make.jsonnet";
local tmc = import "udaqtiming-TimingMasterController-make.jsonnet";
local tpc = import "udaqtiming-TimingPartitionController-make.jsonnet";

local tcmd = import "udaqtiming-timingcmd-make.jsonnet";

local qname = "hardware_commands";     // the name of the single queue in this job


[

    cmd.init([cmd.qspec("hardware_commands", "StdDeQueue", 10)],
             [
              cmd.mspec("thi", "TimingHardwareManager",
                        [cmd.qinfo(thi.hwCmdInQueue, qname, cmd.qdir.input)]),
              cmd.mspec("tmc0", "TimingMasterController",
                        [cmd.qinfo(tmc.hwCmdOutQueue, qname, cmd.qdir.output)]),
              cmd.mspec("tpc0", "TimingPartitionController",
                        [cmd.qinfo(tmc.hwCmdOutQueue, qname, cmd.qdir.output)])
            ]),


    cmd.conf([
              cmd.mcmd("thi", thi.conf("/projects/HEP_Instrumentation/st15719/dunedaq_sw_dev/timing_files/etc/connections.xml")),
              cmd.mcmd("tmc0", tmc.conf("PROD_MASTER")),
              cmd.mcmd("tpc0", tpc.conf("PROD_MASTER", 0)),
              ]),
    
    // send by match-all
    cmd.start(42),

    // send to modules in explicit order
    cmd.stop([cmd.mcmd("thi")]),

    tcmd.master_reset([cmd.mcmd("tmc0")]),
    tcmd.master_set_timestamp([cmd.mcmd("tmc0")]),
    tcmd.master_print_status([cmd.mcmd("tmc0")]),

    tcmd.partition_configure([cmd.mcmd("tpc0")]),
    tcmd.partition_enable([cmd.mcmd("tpc0")]),
    tcmd.partition_disable([cmd.mcmd("tpc0")]),
    tcmd.partition_start([cmd.mcmd("tpc0")]),
    tcmd.partition_stop([cmd.mcmd("tpc0")]),
    tcmd.partition_enable_triggers([cmd.mcmd("tpc0")]),
    tcmd.partition_disable_triggers([cmd.mcmd("tpc0")]),
    tcmd.partition_print_status([cmd.mcmd("tpc0")]),
]


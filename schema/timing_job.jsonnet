local moo = import "moo.jsonnet";

local cmd = import "appfwk-cmd-make.jsonnet";
local thi = import "timing-TimingHardwareManagerPDI-make.jsonnet";
local tmc = import "timing-TimingMasterController-make.jsonnet";
local tpc = import "timing-TimingPartitionController-make.jsonnet";
local tec = import "timing-TimingEndpointController-make.jsonnet";

local tcmd = import "timing-timingcmd-make.jsonnet";

local qname = "hardware_commands";     // the name of the single queue in this job


[

    cmd.init([cmd.qspec("hardware_commands", "StdDeQueue", 10)],
             [
              cmd.mspec("thi", "TimingHardwareManagerPDI",
                        [cmd.qinfo(thi.hwCmdInQueue, qname, cmd.qdir.input)]),
              cmd.mspec("tmc0", "TimingMasterController",
                        [cmd.qinfo(tmc.hwCmdOutQueue, qname, cmd.qdir.output)]),
              cmd.mspec("tpc0", "TimingPartitionController",
                        [cmd.qinfo(tmc.hwCmdOutQueue, qname, cmd.qdir.output)]),
              cmd.mspec("tec0", "TimingEndpointController",
                        [cmd.qinfo(tec.hwCmdOutQueue, qname, cmd.qdir.output)])
            ]),


    cmd.conf([
              cmd.mcmd("thi", thi.conf("${PDT_TESTS}/etc/connections.xml")),
              cmd.mcmd("tmc0", tmc.conf("PROD_MASTER")),
              cmd.mcmd("tpc0", tpc.conf("PROD_MASTER", 0)),
              cmd.mcmd("tec0", tec.conf("EPT_0")),
              ]),
    
    // send by match-all
    cmd.start(42),

    // send to modules in explicit order
    cmd.stop([cmd.mcmd("thi")]),

    tcmd.master_io_reset([cmd.mcmd("tmc0")]),
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

    tcmd.endpoint_io_reset([cmd.mcmd("tec0")]),
    tcmd.endpoint_enable([cmd.mcmd("tec0")]),
    tcmd.endpoint_disable([cmd.mcmd("tec0")]),
    tcmd.endpoint_reset([cmd.mcmd("tec0")]),
//    tcmd.endpoint_print_timestamp([cmd.mcmd("tec0")]),
    tcmd.endpoint_print_status([cmd.mcmd("tec0")]),
    
    tcmd.get_info([cmd.mcmd("thi")])
]


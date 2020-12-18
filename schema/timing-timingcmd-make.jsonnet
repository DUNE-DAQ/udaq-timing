// hand written helpers to make object that match timing-timingcmd-schema.

local appfwkcmd = import "appfwk-cmd-make.jsonnet";

{   
    timingcmdid: { reset:"reset", enable: "enable" },

    timingcmd(id,device) :: {
        id:id, device:device,
    },

    timinghwcmdid: { mastercmd:"mastercmd", partitioncmd: "partitioncmd" },

    timinghwcmd(id,cmd) :: {
        id:id, cmd:cmd,
    },

    master_reset(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("master_reset", addr),
    master_set_timestamp(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("master_set_timestamp", addr),
    master_print_status(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("master_print_status", addr),

    partition_configure(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("partition_configure", addr),
    partition_enable(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("partition_enable", addr),
    partition_disable(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("partition_disable", addr),
    partition_start(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("partition_start", addr),
    partition_stop(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("partition_stop", addr),
    partition_enable_triggers(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("partition_enable_triggers", addr),
    partition_disable_triggers(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("partition_disable_triggers", addr),
    partition_print_status(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("partition_print_status", addr),

}

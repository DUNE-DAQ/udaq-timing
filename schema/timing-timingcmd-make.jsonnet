// hand written helpers to make object that match timing-timingcmd-schema.

local appfwkcmd = import "appfwk-cmd-make.jsonnet";

{
    master_io_reset(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("master_io_reset", addr),
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

    endpoint_io_reset(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("endpoint_io_reset", addr),
    endpoint_enable(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("endpoint_enable", addr),
    endpoint_disable(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("endpoint_disable", addr),
    endpoint_reset(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("endpoint_reset", addr),
    //endpoint_print_timestamp(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("endpoint_print_timestamp", addr),
    endpoint_print_status(addr=appfwkcmd.defaddr) :: appfwkcmd.cmd("endpoint_print_status", addr),

}

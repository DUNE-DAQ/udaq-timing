// hand written helpers to make object compliant with timing-TimingPartitionController-schema
{
    // The internally known name of the queue used
    hwCmdOutQueue: "hardware_commands_out",

    // Make a conf object
    conf(dev="",id=0) :: {
        device: dev,
        partId: id,
    },
}


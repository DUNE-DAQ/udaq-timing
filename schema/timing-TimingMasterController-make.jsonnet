// hand written helpers to make object compliant with timing-TimingMasterController-schema
{
    // The internally known name of the queue used
    hwCmdOutQueue: "hardware_commands_out",

    // Make a conf object
    conf(dev="") :: {
        device: dev,
    },
}


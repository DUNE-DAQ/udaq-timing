// hand written helpers to make object compliant with udaqtiming-TimingHardwareManager-schema
{
    // The internally known name of the queue used
    hwCmdInQueue: "hardware_commands_in",
    
    // Make a conf object
    conf(cf="") :: {
        connectionsFile: cf,
    },
}


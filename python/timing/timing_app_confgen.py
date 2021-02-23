# Set moo schema search path
from dunedaq.env import get_moo_model_path
import moo.io
moo.io.default_load_path = get_moo_model_path()

# Load configuration types
import moo.otypes
moo.otypes.load_types('appfwk/cmd.jsonnet')
moo.otypes.load_types('appfwk/app.jsonnet')
moo.otypes.load_types('cmdlib/cmd.jsonnet')
moo.otypes.load_types('timing/TimingHardwareManagerPDI.jsonnet')
moo.otypes.load_types('timing/TimingMasterController.jsonnet')
moo.otypes.load_types('timing/TimingPartitionController.jsonnet')
moo.otypes.load_types('timing/TimingEndpointController.jsonnet')

# Import new types
import dunedaq.appfwk.cmd as cmd # AddressedCmd, 
import dunedaq.appfwk.app as app # Queue spec
import dunedaq.cmdlib.cmd as cmdlib # Command

import dunedaq.timing.timinghardwaremanagerpdi as thi
import dunedaq.timing.timingmastercontroller as tmc
import dunedaq.timing.timingpartitioncontroller as tpc
import dunedaq.timing.timingendpointcontroller as tec

from appfwk.utils import mcmd, mspec

import json
import math
# Time to waait on pop()
QUEUE_POP_WAIT_MS=100;
# how often do we poll for hardware data [us]
GATHER_INTERVAL=1e6;
def generate(
        RUN_NUMBER = 333, 
        MONITOR_RATE_HZ = 1.0,
        OUTPUT_PATH=".",
    ):
    
    # Define modules and queues
    queue_bare_specs = [
            app.QueueSpec(inst="hardware_commands", kind='StdDeQueue', capacity=100),
        ]

    # Only needed to reproduce the same order as when using jsonnet
    queue_specs = app.QueueSpecs(sorted(queue_bare_specs, key=lambda x: x.inst))

    mod_specs = [
        mspec("thi", "TimingHardwareManagerPDI", [
                        app.QueueInfo(name="hardware_commands_in", inst="hardware_commands", dir="input"),
                    ]),

        mspec("tmc0", "TimingMasterController", [
                        app.QueueInfo(name="hardware_commands_out", inst="hardware_commands", dir="output"),
                    ]),

        mspec("tpc0", "TimingPartitionController", [
                        app.QueueInfo(name="hardware_commands_out", inst="hardware_commands", dir="output"),
                    ]),

        mspec("tec0", "TimingEndpointController", [
                        app.QueueInfo(name="hardware_commands_out", inst="hardware_commands", dir="output"),
                    ]),
        ]

    init_specs = app.Init(queues=queue_specs, modules=mod_specs)

    jstr = json.dumps(init_specs.pod(), indent=4, sort_keys=True)
    print(jstr)

    initcmd = cmdlib.Command(
        id=cmdlib.CmdId("init"),
        data=init_specs
    )


    confcmd = mcmd("conf", [
                ("thi", thi.ConfParams(
                        connections_file="${PDT_TESTS}/etc/connections.xml",
                        gather_interval=GATHER_INTERVAL,
                        monitored_device_name_master="PROD_MASTER",
                        monitored_device_name_endpoint="EPT_0",
                        )),
                ("tmc0", tmc.ConfParams(
                        device="PROD_MASTER",
                        )),
                ("tpc0", tpc.ConfParams(
                        device="PROD_MASTER",
                        partition_id=0,
                        )),
                ("tec0", tec.ConfParams(
                        device="EPT_0",
                        )),
            ])
    
    jstr = json.dumps(confcmd.pod(), indent=4, sort_keys=True)
    print(jstr)

    startpars = cmd.StartParams(run=RUN_NUMBER)
    startcmd = mcmd("start", [
            ("thi", startpars),
            ("tmc.*", startpars),
            ("tpc.*", startpars),
            ("tec.*", startpars),
        ])

    jstr = json.dumps(startcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nStart\n\n", jstr)

    emptypars = cmd.EmptyParams()

    stopcmd = mcmd("stop", [
            ("thi", emptypars),
            ("tmc.*", emptypars),
            ("tpc.*", emptypars),
            ("tec.*", emptypars),
        ])

    jstr = json.dumps(stopcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nStop\n\n", jstr)


    scrapcmd = mcmd("scrap", [
            ("", emptypars)
        ])

    jstr = json.dumps(scrapcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nScrap\n\n", jstr)

    get_info_cmd = mcmd("get_info", [
            ("thi", emptypars),
            ("tmc.*", emptypars),
            ("tpc.*", emptypars),
            ("tec.*", emptypars),
        ])

    ## timing specific commands

    # master commands
    master_io_reset_cmd = mcmd("master_io_reset", [
            ("tmc.*", emptypars),
        ])
    jstr = json.dumps(master_io_reset_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nMaster IO reset\n\n", jstr)


    master_set_timestamp_cmd = mcmd("master_set_timestamp", [
            ("tmc.*", emptypars),
        ])
    jstr = json.dumps(master_set_timestamp_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nMaster set timestamp\n\n", jstr)


    master_print_status_cmd = mcmd("master_print_status", [
            ("tmc.*", emptypars),
        ])
    jstr = json.dumps(master_print_status_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nMaster print status\n\n", jstr)


    # partition commands
    partition_configure_cmd = mcmd("partition_configure", [
            ("tpc.*", emptypars),
        ])
    jstr = json.dumps(partition_configure_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition configure\n\n", jstr)


    partition_enable_cmd = mcmd("partition_enable", [
            ("tpc.*", emptypars),
        ])
    jstr = json.dumps(partition_enable_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition enable\n\n", jstr)


    partition_disable_cmd = mcmd("partition_disable", [
            ("tpc.*", emptypars),
        ])
    jstr = json.dumps(partition_disable_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition disable\n\n", jstr)


    partition_start_cmd = mcmd("partition_start", [
            ("tpc.*", emptypars),
        ])
    jstr = json.dumps(partition_start_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition start\n\n", jstr)


    partition_stop_cmd = mcmd("partition_stop", [
            ("tpc.*", emptypars),
        ])
    jstr = json.dumps(partition_stop_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition stop\n\n", jstr)


    partition_enable_triggers_cmd = mcmd("partition_enable_triggers", [
            ("tpc.*", emptypars),
        ])
    jstr = json.dumps(partition_enable_triggers_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition enable triggers\n\n", jstr)


    partition_disable_triggers_cmd = mcmd("partition_disable_triggers", [
            ("tpc.*", emptypars),
        ])
    jstr = json.dumps(partition_disable_triggers_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition disable triggers\n\n", jstr)


    partition_print_status_cmd = mcmd("partition_print_status", [
            ("tpc.*", emptypars),
        ])
    jstr = json.dumps(partition_print_status_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition print status\n\n", jstr)

    # endpoint commands
    endpoint_io_reset_cmd = mcmd("endpoint_io_reset", [
            ("tec.*", emptypars),
        ])
    jstr = json.dumps(endpoint_io_reset_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nEndpoint IO reset\n\n", jstr)


    endpoint_enable_triggers_cmd = mcmd("endpoint_enable", [
            ("tec.*", emptypars),
        ])
    jstr = json.dumps(endpoint_enable_triggers_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition enable triggers\n\n", jstr)


    endpoint_disable_triggers_cmd = mcmd("endpoint_disable", [
            ("tec.*", emptypars),
        ])
    jstr = json.dumps(endpoint_disable_triggers_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition disable triggers\n\n", jstr)


    endpoint_reset_cmd = mcmd("endpoint_reset", [
            ("tec.*", emptypars),
        ])
    jstr = json.dumps(endpoint_reset_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nEndpoint reset\n\n", jstr)


    endpoint_print_status_cmd = mcmd("endpoint_print_status", [
            ("tec.*", emptypars),
        ])
    jstr = json.dumps(endpoint_print_status_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nEndpoint print status\n\n", jstr)


    # Create a list of commands
    cmd_seq = [initcmd, confcmd, startcmd, stopcmd, get_info_cmd,
            master_io_reset_cmd, master_set_timestamp_cmd, master_print_status_cmd,
            partition_configure_cmd, partition_enable_cmd, partition_disable_cmd, partition_start_cmd, partition_stop_cmd, partition_enable_triggers_cmd, partition_disable_triggers_cmd, partition_print_status_cmd,
            endpoint_io_reset_cmd, endpoint_enable_triggers_cmd, endpoint_disable_triggers_cmd, endpoint_reset_cmd, endpoint_print_status_cmd
        ]

    # Print them as json (to be improved/moved out)
    jstr = json.dumps([c.pod() for c in cmd_seq], indent=4, sort_keys=True)
    return jstr
        
if __name__ == '__main__':
    # Add -h as default help option
    CONTEXT_SETTINGS = dict(help_option_names=['-h', '--help'])

    import click

    @click.command(context_settings=CONTEXT_SETTINGS)
    @click.option('-r', '--run-number', default=333)
    @click.option('-t', '--monitor-rate-hz', default=1.0)
    @click.option('-o', '--output-path', type=click.Path(), default='.')
    @click.argument('json_file', type=click.Path(), default='timing_app.json')
    def cli(run_number, monitor_rate_hz, output_path, json_file):
        """
          JSON_FILE: Input raw data file.
          JSON_FILE: Output json configuration file.
        """

        with open(json_file, 'w') as f:
            f.write(generate(
                    RUN_NUMBER = run_number, 
                    MONITOR_RATE_HZ = monitor_rate_hz,
                    OUTPUT_PATH = output_path,
                ))

        print(f"'{json_file}' generation completed.")

    cli()
    
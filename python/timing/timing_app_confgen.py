# Set moo schema search path
from dunedaq.env import get_moo_model_path
import moo.io
moo.io.default_load_path = get_moo_model_path()

# Load configuration types
import moo.otypes
moo.otypes.load_types('appfwk/cmd.jsonnet')
moo.otypes.load_types('appfwk/app.jsonnet')
moo.otypes.load_types('cmdlib/cmd.jsonnet')
moo.otypes.load_types('rcif/cmd.jsonnet')
moo.otypes.load_types('timing/timinghardwaremanagerpdi.jsonnet')
moo.otypes.load_types('timing/timingmastercontroller.jsonnet')
moo.otypes.load_types('timing/timingpartitioncontroller.jsonnet')
moo.otypes.load_types('timing/timingendpointcontroller.jsonnet')

# Import new types
import dunedaq.appfwk.cmd as cmd # AddressedCmd, 
import dunedaq.appfwk.app as app # Queue spec
import dunedaq.cmdlib.cmd as cmdlib # Command
import dunedaq.rcif.cmd as rcif # rcif

import dunedaq.timing.timinghardwaremanagerpdi as thi
import dunedaq.timing.timingmastercontroller as tmc
import dunedaq.timing.timingpartitioncontroller as tpc
import dunedaq.timing.timingendpointcontroller as tec

from appfwk.utils import mcmd, mspec, mrccmd

import json
import math

def generate(
        RUN_NUMBER = 333, 
        GATHER_INTERVAL = 1e6,
        GATHER_INTERVAL_DEBUG = 10e6,
        MASTER_DEVICE_NAME="PROD_MASTER",
        ENDPOINT_DEVICE_NAME="",
        UHAL_LOG_LEVEL="notice",
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

    initcmd = rcif.RCCommand(
        id=cmdlib.CmdId("init"),
        entry_state="NONE",
        exit_state="INITIAL",
        data=init_specs
    )


    confcmd = mrccmd("conf", "INITIAL", "CONFIGURED",[
                ("thi", thi.ConfParams(
                        connections_file="${PDT_TESTS}/etc/connections.xml",
                        gather_interval=GATHER_INTERVAL,
                        gather_interval_debug=GATHER_INTERVAL_DEBUG,
                        monitored_device_name_master=MASTER_DEVICE_NAME,
                        monitored_device_name_endpoint=ENDPOINT_DEVICE_NAME,
                        uhal_log_level=UHAL_LOG_LEVEL
                        )),
                ("tmc0", tmc.ConfParams(
                        device=MASTER_DEVICE_NAME,
                        )),
                ("tpc0", tpc.ConfParams(
                        device=MASTER_DEVICE_NAME,
                        partition_id=0,
                        )),
                ("tec0", tec.ConfParams(
                        device=ENDPOINT_DEVICE_NAME,
                        )),
            ])
    
    jstr = json.dumps(confcmd.pod(), indent=4, sort_keys=True)
    print(jstr)

    startcmd = mrccmd("start", "CONFIGURED", "RUNNING", [
            ("thi", None),
            ("tmc.*", None),
            ("tpc.*", None),
            ("tec.*", None),
        ])

    jstr = json.dumps(startcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nStart\n\n", jstr)

    stopcmd = mrccmd("stop", "RUNNING", "CONFIGURED", [
            ("thi", None),
            ("tmc.*", None),
            ("tpc.*", None),
            ("tec.*", None),
        ])

    jstr = json.dumps(stopcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nStop\n\n", jstr)


    scrapcmd = mcmd("scrap", [
            ("", None)
        ])

    jstr = json.dumps(scrapcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nScrap\n\n", jstr)

    get_info_cmd = mcmd("get_info", [
            ("thi", None),
            ("tmc.*", None),
            ("tpc.*", None),
            ("tec.*", None),
        ])

    ## timing specific commands

    # master commands
    master_io_reset_cmd = mcmd("master_io_reset", [
            ("tmc.*", None),
        ])
    jstr = json.dumps(master_io_reset_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nMaster IO reset\n\n", jstr)


    master_set_timestamp_cmd = mcmd("master_set_timestamp", [
            ("tmc.*", None),
        ])
    jstr = json.dumps(master_set_timestamp_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nMaster set timestamp\n\n", jstr)


    master_print_status_cmd = mcmd("master_print_status", [
            ("tmc.*", None),
        ])
    jstr = json.dumps(master_print_status_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nMaster print status\n\n", jstr)


    # partition commands
    partition_configure_cmd = mcmd("partition_configure", [
            ("tpc.*", None),
        ])
    jstr = json.dumps(partition_configure_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition configure\n\n", jstr)


    partition_enable_cmd = mcmd("partition_enable", [
            ("tpc.*", None),
        ])
    jstr = json.dumps(partition_enable_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition enable\n\n", jstr)


    partition_disable_cmd = mcmd("partition_disable", [
            ("tpc.*", None),
        ])
    jstr = json.dumps(partition_disable_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition disable\n\n", jstr)


    partition_start_cmd = mcmd("partition_start", [
            ("tpc.*", None),
        ])
    jstr = json.dumps(partition_start_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition start\n\n", jstr)


    partition_stop_cmd = mcmd("partition_stop", [
            ("tpc.*", None),
        ])
    jstr = json.dumps(partition_stop_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition stop\n\n", jstr)


    partition_enable_triggers_cmd = mcmd("partition_enable_triggers", [
            ("tpc.*", None),
        ])
    jstr = json.dumps(partition_enable_triggers_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition enable triggers\n\n", jstr)


    partition_disable_triggers_cmd = mcmd("partition_disable_triggers", [
            ("tpc.*", None),
        ])
    jstr = json.dumps(partition_disable_triggers_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition disable triggers\n\n", jstr)


    partition_print_status_cmd = mcmd("partition_print_status", [
            ("tpc.*", None),
        ])
    jstr = json.dumps(partition_print_status_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition print status\n\n", jstr)

    # endpoint commands
    endpoint_io_reset_cmd = mcmd("endpoint_io_reset", [
            ("tec.*", None),
        ])
    jstr = json.dumps(endpoint_io_reset_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nEndpoint IO reset\n\n", jstr)


    endpoint_enable_triggers_cmd = mcmd("endpoint_enable", [
            ("tec.*", None),
        ])
    jstr = json.dumps(endpoint_enable_triggers_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition enable triggers\n\n", jstr)


    endpoint_disable_triggers_cmd = mcmd("endpoint_disable", [
            ("tec.*", None),
        ])
    jstr = json.dumps(endpoint_disable_triggers_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nPartition disable triggers\n\n", jstr)


    endpoint_reset_cmd = mcmd("endpoint_reset", [
            ("tec.*", None),
        ])
    jstr = json.dumps(endpoint_reset_cmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nEndpoint reset\n\n", jstr)


    endpoint_print_status_cmd = mcmd("endpoint_print_status", [
            ("tec.*", None),
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
    @click.option('-g', '--gather-interval', default=1e6)
    @click.option('-d', '--gather-interval-debug', default=10e6)
    @click.option('-m', '--master-device-name', default="PROD_MASTER")
    @click.option('-e', '--endpoint-device-name', default="")
    @click.option('-u', '--uhal-log-level', default="notice")
    @click.option('-o', '--output-path', type=click.Path(), default='.')
    @click.argument('json_file', type=click.Path(), default='timing_app.json')
    def cli(run_number, gather_interval, gather_interval_debug, master_device_name, endpoint_device_name, uhal_log_level, output_path, json_file):
        """
          JSON_FILE: Input raw data file.
          JSON_FILE: Output json configuration file.
        """

        with open(json_file, 'w') as f:
            f.write(generate(
                    RUN_NUMBER = run_number, 
                    GATHER_INTERVAL = gather_interval,
                    GATHER_INTERVAL_DEBUG = gather_interval_debug,
                    MASTER_DEVICE_NAME = master_device_name,
                    ENDPOINT_DEVICE_NAME = endpoint_device_name,
                    UHAL_LOG_LEVEL = uhal_log_level,
                    OUTPUT_PATH = output_path,
                ))

        print(f"'{json_file}' generation completed.")

    cli()
    
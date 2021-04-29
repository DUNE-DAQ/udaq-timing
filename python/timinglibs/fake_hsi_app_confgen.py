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
moo.otypes.load_types('timinglibs/fakehsieventgenerator.jsonnet')
moo.otypes.load_types('nwqueueadapters/queuetonetwork.jsonnet')
moo.otypes.load_types('nwqueueadapters/networktoqueue.jsonnet')
moo.otypes.load_types('nwqueueadapters/networkobjectreceiver.jsonnet')
moo.otypes.load_types('nwqueueadapters/networkobjectsender.jsonnet')

# Import new types
import dunedaq.appfwk.cmd as cmd # AddressedCmd, 
import dunedaq.appfwk.app as app # Queue spec
import dunedaq.cmdlib.cmd as cmdlib # Command
import dunedaq.rcif.cmd as rcif # rcif

import dunedaq.timinglibs.fakehsieventgenerator as fhsig
import dunedaq.nwqueueadapters.networktoqueue as ntoq
import dunedaq.nwqueueadapters.queuetonetwork as qton
import dunedaq.nwqueueadapters.networkobjectreceiver as nor
import dunedaq.nwqueueadapters.networkobjectsender as nos

from appfwk.utils import mcmd, mspec, mrccmd

import json
import math

def generate(
        OUTPUT_PATH=".",
        CLOCK_SPEED_HZ: int = 50000000,
        HSI_EVENT_PERIOD_NS: int = 20,
        HSI_DEVICE_ID: int = 0,
        MEAN_SIGNAL_MULTIPLICITY: int = 0,
        SIGNAL_EMULATION_MODE: int = 0,
        ENABLED_SIGNALS: int = 0b00000001,
    ):

    network_endpoints={
        "timesync": "tcp://localhost:12347"
    }
    
    # Define modules and queues
    queue_bare_specs = [
            app.QueueSpec(inst="time_sync_from_netq", kind='FollySPSCQueue', capacity=100),
            app.QueueSpec(inst="hsievent_q_to_net", kind='FollySPSCQueue', capacity=100e6),
        ]

    # Only needed to reproduce the same order as when using jsonnet
    queue_specs = app.QueueSpecs(sorted(queue_bare_specs, key=lambda x: x.inst))

    mod_specs = [
                    mspec("ntoq_timesync", "NetworkToQueue", [
                        app.QueueInfo(name="output", inst="time_sync_from_netq", dir="output")
                    ]),

                    mspec("fhsig", "FakeHSIEventGenerator", [
                        app.QueueInfo(name="time_sync_source", inst="time_sync_from_netq", dir="input"),
                        app.QueueInfo(name="hsievent_sink", inst="hsievent_q_to_net", dir="output"),
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

    mods = [
                ("ntoq_timesync", ntoq.Conf(msg_type="dunedaq::dfmessages::TimeSync",
                                           msg_module_name="TimeSyncNQ",
                                           receiver_config=nor.Conf(ipm_plugin_type="ZmqReceiver",
                                                                    address=network_endpoints["timesync"])
                                           )
                ),

                ("fhsig", fhsig.Conf(
                        clock_frequency=CLOCK_SPEED_HZ,
                        event_period=HSI_EVENT_PERIOD_NS,
                        mean_signal_multiplicity=MEAN_SIGNAL_MULTIPLICITY,
                        signal_emulation_mode=SIGNAL_EMULATION_MODE,
                        enabled_signals=ENABLED_SIGNALS,
                        )),
            ]

    confcmd = mrccmd("conf", "INITIAL", "CONFIGURED", mods)

    jstr = json.dumps(confcmd.pod(), indent=4, sort_keys=True)
    print(jstr)

    startpars = rcif.StartParams(run=33, disable_data_storage=False)

    startcmd = mrccmd("start", "CONFIGURED", "RUNNING", [
            ("ntoq_timesync", startpars),
            ("fhsig", startpars)
        ])

    jstr = json.dumps(startcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nStart\n\n", jstr)

    stopcmd = mrccmd("stop", "RUNNING", "CONFIGURED", [
            ("ntoq_timesync", None),
            ("fhsig", None)
        ])

    jstr = json.dumps(stopcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nStop\n\n", jstr)


    scrapcmd = mcmd("scrap", [
            ("", None)
        ])

    jstr = json.dumps(scrapcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nScrap\n\n", jstr)


    # Create a list of commands
    cmd_seq = [initcmd, confcmd, startcmd, stopcmd]

    # Print them as json (to be improved/moved out)
    jstr = json.dumps([c.pod() for c in cmd_seq], indent=4, sort_keys=True)
    return jstr
        
if __name__ == '__main__':
    # Add -h as default help option
    CONTEXT_SETTINGS = dict(help_option_names=['-h', '--help'])

    import click

    @click.command(context_settings=CONTEXT_SETTINGS)
    @click.option('-o', '--output-path', type=click.Path(), default='.')
    @click.argument('json_file', type=click.Path(), default='fake_hsi_app.json')
    def cli(output_path, json_file):
        """
          JSON_FILE: Input raw data file.
          JSON_FILE: Output json configuration file.
        """

        with open(json_file, 'w') as f:
            f.write(generate(
                    OUTPUT_PATH = output_path,
                ))

        print(f"'{json_file}' generation completed.")

    cli()
    
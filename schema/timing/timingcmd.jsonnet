local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.timing.timingcmd";
local s = moo.oschema.schema(ns);

// A temporary schema construction context.
local timingcmd = {
    
    uint_data: s.number("UintData", "u4", 
        doc="A PLL register bit(s) value"),

    inst: s.string("InstName", moo.re.ident_only,
                   doc="Name of a target instance of a kind"),

    timinghwcmdid: s.string("TimingHwCmdId", pattern=moo.re.ident_only,
                    doc="The timing hw cmd name.  FIXME: this should be an enum!"),

    timing_hw_cmd_payload: s.any("TimingHwCmdPayload", 
                    doc="Generic structure for timing hw cmd payloads"),

    timinghwcmd: s.record("TimingHwCmd", [
        s.field("id", self.timinghwcmdid,
                doc="ID of hw cmd"),
        s.field("device", self.inst,
                doc="Cmd target"),
        s.field("payload", self.timing_hw_cmd_payload,
                doc="Hw cmd payload")

    ], doc="Timing hw cmd structure"),

    timing_partition_cmd_payload: s.record("TimingPartitionCmdPayload",[
        s.field("partition_id", self.uint_data,
            doc="ID of target partition")
    ],
    doc="Structure for payload of timing partition commands")
};

// Output a topologically sorted array.
moo.oschema.sort_select(timingcmd, ns)

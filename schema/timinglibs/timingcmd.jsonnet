local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.timinglibs.timingcmd";
local s = moo.oschema.schema(ns);

// A temporary schema construction context.
local timingcmd = {
    
    bool_data: s.boolean("BoolData", doc="A bool"),

    uint_data: s.number("UintData", "u4", 
        doc="A PLL register bit(s) value"),

    inst: s.string("String",
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

    io_reset_cmd_payload: s.record("IOResetCmdPayload",[
        s.field("clock_config", self.inst, "",
            doc="Path of clock config file"),
        s.field("soft", self.bool_data, false,
            doc="Soft reset"),
        s.field("fanout_mode", self.uint_data, 0, 
            doc="Fanout mode. 0: fanout, 1: standalone"),
    ], doc="Structure for io reset commands"),

    timing_partition_cmd_payload: s.record("TimingPartitionCmdPayload",[
        s.field("partition_id", self.uint_data,
            doc="ID of target partition"),
    ], doc="Structure for payload of partition commands"),

    timing_partition_configure_cmd_payload: s.record("TimingPartitionConfigureCmdPayload",[
        s.field("partition_id", self.uint_data, optional=true,
            doc="ID of target partition"),
        s.field("trigger_mask", self.uint_data,
            doc="Trigger mask for fixed length cmd distribution"),
        s.field("spill_gate_enabled", self.bool_data, false,
            doc="Spill interface on"),
        s.field("rate_control_enabled", self.bool_data, false,
            doc="Rate control on"),
    ], doc="Structure for payload of partition configure commands"),

    timing_endpoint_cmd_payload: s.record("TimingEndpointCmdPayload",[
        s.field("endpoint_id", self.uint_data,
            doc="ID of target endpoint"),
    ], doc="Structure for payload of endpoint commands"),

    timing_endpoint_configure_cmd_payload: s.record("TimingEndpointConfigureCmdPayload",[
        s.field("endpoint_id", self.uint_data, optional=true,
            doc="ID of target endpoint"),
        s.field("address", self.uint_data,
            doc="Endpoint address"),
        s.field("partition", self.uint_data,
            doc="Endpoint partition"),
    ], doc="Structure for payload of endpoint configure commands"),

    hsi_configure_cmd_payload: s.record("HSIConfigureCmdPayload",[
        s.field("rising_edge_mask", self.uint_data,
            doc="Rising edge mask for HSI triggering"),

        s.field("falling_edge_mask", self.uint_data,
            doc="Falling edge mask for HSI triggering"),
        
        s.field("invert_edge_mask", self.uint_data,
            doc="Invert edge mask for HSI triggering"),
        
        s.field("data_source", self.uint_data,
            doc="Source of data for HSI triggering"),
    ], doc="Structure for payload of hsi configure commands")
};

// Output a topologically sorted array.
moo.oschema.sort_select(timingcmd, ns)

local moo = import "moo.jsonnet";
local ns = "dunedaq.timinglibs.timingpartitioncontroller";
local s = moo.oschema.schema(ns);

local s_app = import "appfwk/app.jsonnet";
local app = moo.oschema.hier(s_app).dunedaq.appfwk.app;

local cs = {
	
    str : s.string("Str", doc="A string field"),

    uint_data: s.number("UintData", "u4",
        doc="A count of very many things"),
    
    bool_data: s.boolean("BoolData", doc="A bool"),

    init: s.record("InitParams", [
        s.field("qinfos", app.QueueInfos,
                doc="Information for a module to find its queue"),
        s.field("device", self.str, "",
                doc="String of managed device name"),
        s.field("partition_id", self.uint_data, 0,
                doc="Part id number"),
    ], doc="TimingPartitionController init parameters"),

    conf: s.record("PartitionConfParams", [
        s.field("trigger_mask", self.uint_data,
            doc="Trigger mask for fixed length cmd distribution"),
        s.field("spill_gate_enabled", self.bool_data, false,
            doc="Spill interface on"),
        s.field("rate_control_enabled", self.bool_data, false,
            doc="Rate control on"),
    ], doc="TimingPartitionController configuration parameters"),

};

s_app + moo.oschema.sort_select(cs)
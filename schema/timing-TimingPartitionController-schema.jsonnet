local moo = import "moo.jsonnet";
local ns = "dunedaq.timing.timingpartitioncontroller";
local s = moo.oschema.schema(ns);

local types = {
	
    str : s.string("Str", doc="A string field"),

    uint_data: s.number("UintData", "u4",
        doc="A count of very many things"),

    conf: s.record("ConfParams", [
        s.field("device", self.str, "",
                doc="String of managed device name"),
        s.field("partition_id", self.uint_data, 0,
                doc="Part id number"),
    ], doc="TimingPartitionController configuration"),

};

moo.oschema.sort_select(types, ns)

local moo = import "moo.jsonnet";
local ns = "dunedaq.timinglibs.timingendpointcontroller";
local s = moo.oschema.schema(ns);

local types = {
	
    str : s.string("Str", doc="A string field"),
    
    size: s.number("Size", "u8",
        doc="A count of very many things"),

    uint_data: s.number("UintData", "u4",
        doc="A count of very many things"),

    conf: s.record("ConfParams", [
        s.field("device", self.str, "",
                doc="String of managed device name"),
    ], doc="TimingEndpointController configuration"),

};

moo.oschema.sort_select(types, ns)

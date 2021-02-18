local moo = import "moo.jsonnet";
local ns = "dunedaq.timing.timinghardwaremanagerpdi";
local s = moo.oschema.schema(ns);

local types = {
    size: s.number("Size", "u8",
                   doc="A count of very many things"),

    count : s.number("Count", "i4",
                     doc="A count of not too many things"),

    str : s.string("Str", doc="A string field"),

    conf: s.record("ConfParams", [
        s.field("connections_file", self.str, "",
                doc="device connections file"),
    ], doc="TimingHardwareManager configuration"),

};

moo.oschema.sort_select(types, ns)

local moo = import "moo.jsonnet";
local ns = "dunedaq.udaqtiming.timinghardwaremanager";
local s = moo.oschema.schema(ns);

local types = {
    size: s.number("Size", "u8",
                   doc="A count of very many things"),

    count : s.number("Count", "i4",
                     doc="A count of not too many things"),

    str : s.string("Str", "string",
                   doc="A string field"),

    conf: s.record("Conf", [
        s.field("connectionsFile", self.str, "",
                doc="device connections file"),
    ], doc="TimingHardwareManager configuration"),

};

moo.oschema.sort_select(types, ns)

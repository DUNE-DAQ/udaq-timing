local moo = import "moo.jsonnet";
local ns = "dunedaq.timing.timinghardwaremanagerpdi";
local s = moo.oschema.schema(ns);

local types = {
    uint_data: s.number("UintData", "u4",
        doc="A count of very many things"),

    count : s.number("Count", "i4",
        doc="A count of not too many things"),

    str : s.string("Str", doc="A string field"),

    conf: s.record("ConfParams", [
        s.field("connections_file", self.str, "",
                doc="device connections file"),
        s.field("gather_interval", self.uint_data, 1e6,
                doc="Hardware device data gather interval [us]"),
        s.field("monitored_device_name_master", self.str, "",
                doc="Name of timing master device to be monitored"),
        s.field("monitored_device_name_endpoint", self.str, "",
                doc="Name of timing endpoint device to be monitored")
    ], doc="TimingHardwareManager configuration"),

};

moo.oschema.sort_select(types, ns)

local moo = import "moo.jsonnet";
local ns = "dunedaq.timing.timingmastercontroller";
local s = moo.oschema.schema(ns);

local types = {

    str : s.string("Str", "string",
                   doc="A string field"),

    conf: s.record("Conf", [
        s.field("device", self.str, "",
                doc="String of managed device name"),
    ], doc="TimingMasterController configuration"),

};

moo.oschema.sort_select(types, ns)

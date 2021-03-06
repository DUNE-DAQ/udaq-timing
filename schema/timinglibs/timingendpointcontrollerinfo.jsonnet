local moo = import "moo.jsonnet";
local s = moo.oschema.schema("dunedaq.timinglibs.timingendpointcontrollerinfo");

local info = {
   cl : s.string("class_s", moo.re.ident,
                  doc="A string field"), 
    uint8  : s.number("uint8", "u8",
                     doc="An unsigned of 8 bytes"),

    counter_vector: s.sequence("HwCommandCounters", self.uint8,
            doc="A vector hardware command counters"),

   info: s.record("Info", [
       s.field("sent_hw_command_counters", self.counter_vector, doc="Number of hw commands sent so far"), 
   ], doc="TimingEndpointController information")
};

moo.oschema.sort_select(info)
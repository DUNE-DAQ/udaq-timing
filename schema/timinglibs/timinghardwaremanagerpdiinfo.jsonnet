local moo = import "moo.jsonnet";
local s = moo.oschema.schema("dunedaq.timinglibs.timinghardwaremanagerpdiinfo");

local info = {
   cl : s.string("class_s", moo.re.ident,
                  doc="A string field"), 
    uint8  : s.number("uint8", "u8",
                     doc="An unsigned of 8 bytes"),


   timing_device_hw_info: s.any("TimingHwInfo",
                    doc="Generic structure for timing hw op mon info payload"),

   info: s.record("Info", [
       s.field("received_hw_commands_counter", self.uint8, 0, doc="Number of hw commands received so far"), 
       s.field("accepted_hw_commands_counter", self.uint8, 0, doc="Number of hw commands accepted so far"), 
       s.field("rejected_hw_commands_counter", self.uint8, 0, doc="Number of hw commands rejected so far"), 
       s.field("failed_hw_commands_counter", self.uint8, 0, doc="Number of hw commands rejected so far"),
       s.field("master_info", self.timing_device_hw_info, doc="Timing master hw info"),
   ], doc="TimingHardwareManagerPDI information")
};

moo.oschema.sort_select(info)
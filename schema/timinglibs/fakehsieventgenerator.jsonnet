local moo = import "moo.jsonnet";
local ns = "dunedaq.timinglibs.fakehsieventgenerator";
local s = moo.oschema.schema(ns);

local types = {
    dbl: s.number("Dbl", dtype="f8"),

    u64: s.number("U64", dtype="u8"),

    u32: s.number("U32", dtype="u4"),

    conf: s.record("Conf", [

      s.field("clock_frequency", self.u64, 50000000,
        doc="Assumed clock frequency in Hz (for current-timestamp estimation)"),

      s.field("event_period", self.u32, 50000000,
        doc="Period between HSIEvent generation"),

      s.field("hsi_device_id", self.u32, 1,
        doc="HSI device ID for emulated HSIEvent messages"),

      s.field("mean_signal_multiplicity", self.u32, 1,
        doc="Mean number of edges expected per signal. Used when signal emulation mode is 1"),

      s.field("enabled_signals", self.u32, 0,
        doc="Which signals or bit of the 32 bit signal bit map are enabled, i.e. could produce an emulated signal"),

      s.field("signal_emulation_mode", self.u32, 0,
        doc="Signal bit map emulation mode. 0: enabled signals always on; 1: enabled signals are emulated (independently) on according to a Poisson with mean mean_signal_multiplicity; signal map generated with uniform distr. enabled signals only"),

    ], doc="FakeHSIEventoGenerator configuration parameters"),

};

moo.oschema.sort_select(types, ns)
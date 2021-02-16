local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.timing.timingmon";
local s = moo.oschema.schema(ns);

// A temporary schema construction context.
local timingmon = {
    
    pll_config_id: s.string("PLLConfigID", moo.re.ident_only, 
        doc="A string field"),

    pll_reg_val: s.number("PLLRegValue", "u2", 
        doc="A PLL register bit(s) value"),

    timing_pll_mon_data: s.record("TimingPLLMonitorData", 
   	[
        s.field("config_id", self.pll_reg_val,
                doc="PLL config ID"),
        s.field("cal_pll", self.pll_reg_val,
                doc="Cal pll"),
        s.field("hold", self.pll_reg_val,
                doc="Holdover flag"),
        s.field("lol", self.pll_reg_val,
                doc="Loss of lock flag"),
        s.field("los", self.pll_reg_val,
                doc="Loss of signal flag"),
        s.field("los_xaxb", self.pll_reg_val,
                doc="Loss of signal flag XAXB"),
        s.field("los_xaxb_flg", self.pll_reg_val,
                doc="Loss of signal flag XAXB stricky"),
        s.field("oof", self.pll_reg_val,
                doc="Out of frequency flags"),
        s.field("oof_sticky", self.pll_reg_val,
                doc="Out of frequency flags sticky"),
        s.field("smbus_timeout", self.pll_reg_val,
                doc="SMBUS timeout"),
        s.field("smbus_timeout_flg", self.pll_reg_val,
                doc="SMBUS timeout sticky"),
        s.field("sysincal", self.pll_reg_val,
               doc="In calibration flag"),
        s.field("sysincal_flg", self.pll_reg_val,
                doc="In calibration flag sticky"),
        s.field("xaxb_err", self.pll_reg_val,
                doc="XA-XB error flag"),
        s.field("xaxb_err_flg", self.pll_reg_val,
                doc="XA-XB error flag sticky"),
    ], 
    doc="Timing PLL monitor structure"),
};

// Output a topologically sorted array.
moo.oschema.sort_select(timingmon, ns)

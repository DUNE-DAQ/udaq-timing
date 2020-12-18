local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.timing.timingcmd";
local s = moo.oschema.schema(ns);

// A temporary schema construction context.
local timingcmd = {
    
    inst: s.string("InstName", moo.re.ident_only,
                   doc="Name of a target instance of a kind"),

    timingcmdid: s.string("TimingCmdId", pattern=moo.re.ident_only,
                    doc="The timing command name.  FIXME: this should be an enum!"),

    timingcmd: s.record("TimingCmd", [
        s.field("id", self.timingcmdid,
                doc="Cmd id"),
        s.field("device", self.inst,
                doc="Cmd target"),
    ], doc="Timing cmd structure"),


    timinghwcmdid: s.string("TimingHwCmdId", pattern=moo.re.ident_only,
                    doc="The timing hw cmd name.  FIXME: this should be an enum!"),

    timinghwcmd: s.record("TimingHwCmd", [
        s.field("id", self.timinghwcmdid,
                doc="ID of hw cmd"),
        s.field("cmd", self.timingcmd,
                doc="Hw cmd payload: Timing cmd"),
    ], doc="Timing hw cmd structure"),
};

// Output a topologically sorted array.
moo.oschema.sort_select(timingcmd, ns)

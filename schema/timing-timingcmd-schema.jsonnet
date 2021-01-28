local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.timing.timingcmd";
local s = moo.oschema.schema(ns);

// A temporary schema construction context.
local timingcmd = {
    
    inst: s.string("InstName", moo.re.ident_only,
                   doc="Name of a target instance of a kind"),

    timinghwcmdid: s.string("TimingHwCmdId", pattern=moo.re.ident_only,
                    doc="The timing hw cmd name.  FIXME: this should be an enum!"),

    timinghwcmd: s.record("TimingHwCmd", [
        s.field("id", self.timinghwcmdid,
                doc="ID of hw cmd"),
        s.field("device", self.inst,
                doc="Cmd target"),
    ], doc="Timing hw cmd structure"),
};

// Output a topologically sorted array.
moo.oschema.sort_select(timingcmd, ns)

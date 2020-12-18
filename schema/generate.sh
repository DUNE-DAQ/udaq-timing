#!/usr/bin/env bash

mydir=$(dirname $(realpath $BASH_SOURCE))
srcdir=$(dirname $mydir)

set -x

render () {
    local name="$1" ; shift
    local What="$1" ; shift

    local name_lc=$( echo "$name" | tr '[:upper:]' '[:lower:]' )
    local outdir="${1:-$srcdir/include/udaqtiming/${name}}"
    local what="$(echo $What | tr '[:upper:]' '[:lower:]')"
    local tmpl="o${what}.hpp.j2"
    local outhpp="$outdir/${What}.hpp"

    mkdir -p $outdir
    moo -g '/lang:ocpp.jsonnet' \
        -M $mydir -T $mydir \
        -A path="dunedaq.udaqtiming.${name_lc}" \
        -A ctxpath="dunedaq" \
        -A os="udaqtiming-${name}-schema.jsonnet" \
        render omodel.jsonnet $tmpl \
        > $outhpp || exit -1
    echo $outhpp
}


render timingcmd Structs
render timingcmd Nljs
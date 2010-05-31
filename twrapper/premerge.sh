#!/bin/bash
if [[ -n "${1}" ]]; then
    emerge -f $@
    ./twrapper.py
#    emerge $@
else
    echo
    echo "Usage:"
    echo "   ./premerge.sh pkg_name"
    echo "Example:"
    echo "   ./premerge.sh vlc"
fi
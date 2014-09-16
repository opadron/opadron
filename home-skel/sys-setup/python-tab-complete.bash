#! /usr/bin/env bash

python_startup_file="$PYTHONSTARTUP"
has_python_startup_file=1

if [ -z "$python_startup_file" ] ; then
    python_startup_file="$HOME/.pythonrc.py"
    has_python_startup_file=0
fi

cat >> "$python_startup_file" << EOF
try:
    import readline
except ImportError:
    print "Module readline not available."
else:
    import rlcompleter
    readline.parse_and_bind("tab: complete")
EOF

if ((!has_python_startup_file)) ; then
    cat >> $HOME/.bashrc << ____EOF
export PYTHONSTARTUP="\$HOME/.pythonrc.py"
____EOF
fi


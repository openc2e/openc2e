#!/bin/bash
echo "digraph {"
#for i in *.h *.hpp; do
#    echo -n $i | sed 's/\.//g'
#    echo " [label=\"$i\"];"
#done
grep '^[ \t]*#include "' "$@"|sed 's/^\([^:]*\):[ \t]*#include "\([^"]*\)".*$/"\2" -> "\1";/'

echo }

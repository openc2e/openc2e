#!/bin/bash
echo "digraph {"
for i in "$@"; do echo "\"$i\";"; done
grep '^[ \t]*#include [<"]' "$@"|perl -ne 'm{^([^:]*):[ \t]*#include [<"]([^>"]*)[>"].*$}; -e $2 && print qq{"$2" -> "$1"};'|sort|uniq

echo }

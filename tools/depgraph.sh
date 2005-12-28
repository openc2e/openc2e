#!/bin/bash
echo "digraph {"
for i in "$@"; do echo "\"$i\";"; done
grep '^[ \t]*#include "' "$@"|sed 's/^\([^:]*\):[ \t]*#include "\([^"]*\)".*$/"\2" -> "\1";/'|sort|uniq

echo }

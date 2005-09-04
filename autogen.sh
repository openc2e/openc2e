#!/bin/sh
AM_VERS=-1.9
AC_VERS=2.50

cat m4/* > acinclude.m4 &&
libtoolize --force &&
aclocal$AM_VERS &&
autoheader$AC_VERS &&
automake$AM_VERS --foreign --add-missing &&
autoconf$AC_VERS &&
echo Ok, all done. Now type ./configure &&
echo For debug, source debug-env.sh before configuring or making.

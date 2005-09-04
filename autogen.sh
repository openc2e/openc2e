#!/bin/sh
if [ -x /usr/bin/automake-1.9 ]
then
    AM_VERS=-1.9
else
    AM_VERS=
fi
if [ -x /usr/bin/autoheader2.50 ]
then
    AC_VERS=2.50
else
    AC_VERS=
fi

cat m4/* > acinclude.m4 &&
libtoolize --force &&
aclocal$AM_VERS &&
autoheader$AC_VERS &&
automake$AM_VERS --foreign --add-missing &&
autoconf$AC_VERS &&
echo Ok, all done. Now type ./configure &&
echo For debug, source debug-env.sh before configuring or making.

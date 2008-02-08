#!/bin/zsh -e
# vim: set ft=zsh :

trunk='svn://ccdevnet.org/openc2e/trunk'
ppasuffix="ppa0"
#set -A suites hardy gutsy
set -A suites gutsy

rev=`svn info $trunk|grep '^Revision:'|sed 's/.*: //'`
tempdir=`mktemp -d`

pushd $tempdir
echo Building package in $tempdir...
checkout=openc2e-0.0svn$rev

svn export $trunk $checkout -r $rev
tar czvf openc2e_0.0svn$rev.orig.tar.gz $checkout
cp $checkout/debian/changelog $tempdir/changelog.orig

pushd $checkout/debian
cp -f $tempdir/changelog.orig changelog
debversion="0.0svn${rev}-1~${ppasuffix}"
debchange -v $debversion -D UNRELEASED -b "Automatic checkout of revision ${rev}"
cd ..
dpkg-buildpackage -sa -rfakeroot -S
popd

for suite in $suites; do
	sed "s/SUITE/$suite/" < $tempdir/$checkout/debian/dput.cf > $tempdir/dput.cf
	dput -c $tempdir/dput.cf ccdevnet-ppa openc2e_${debversion}_source.changes
done

rm -rf $tempdir

#!/bin/zsh -e
# vim: set ft=zsh :

trunk='svn://ccdevnet.org/openc2e/trunk'
ppasuffix="ppa0"
## These should be in order from earliest ubuntu release to latest
set -A dists hardy gutsy

rev=`svn info $trunk|grep '^Revision:'|sed 's/.*: //'`
tempdir=`mktemp -d`

pushd $tempdir
echo Building package in $tempdir...
checkout=openc2e-0.0svn$rev

svn export $trunk $checkout -r $rev
tar czvf openc2e_0.0svn$rev.orig.tar.gz $checkout
cp $checkout/debian/changelog $tempdir/changelog.orig
distindex=0
for dist in $dists; do
	distindex=$(($distindex + 1))
	pushd $checkout/debian
	cp -f $tempdir/changelog.orig changelog
	debversion="0.0svn${rev}-1~${ppasuffix}-${distindex}${dist}"
	debchange -v $debversion -D $dist -b "Automatic checkout of revision ${rev}"
	cd ..
	dpkg-buildpackage -sa -rfakeroot -S
	popd
	dput -c $tempdir/$checkout/debian/dput.cf ccdevnet-ppa openc2e_${debversion}_source.changes
done

rm -rf $tempdir

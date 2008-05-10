#!/usr/bin/perl
use strict;
use warnings;

# Note: This is for C/C++ files only.

sub min {
    my $n = shift @_;
    for (@_) {
        if ($_ < $n) {
            $n = $_;
        }
    }
    return $n;
}

sub max {
    my $n = shift @_;
    for (@_) {
        if ($_ > $n) {
            $n = $_;
        }
    }
    return $n;
}

open AUTHORS, "<AUTHORS" or die "Can't open AUTHORS file: $!";
my @AUTHORS = <AUTHORS>;
close AUTHORS;

my %authors;

for (@AUTHORS) {
    if (m{^([^-].*) <([^>]+)> \((.*)\)$}) {
        $authors{$3} = $1;
    }
}       

for my $file (@ARGV) {
    sleep 2;
    print "Processing $file...\n";
    my @log = `svn log -q "$file"`;
    @log = grep { /^r/ } @log;
    if (!@log) {
        print STDERR "No svn log information found for $file! Skipping...\n";
        next;
    }

    my %fauthors;

    my ($orig_author, $orig_date);
    
    for my $entry (@log) {
#        print $entry;
        unless ($entry =~ /^r\d+ \| (.+) \| (\d{4}).* \((.*)\)/) {
            printf STDERR "Malformed svn output: $entry\n";
            exit 1;
        }
        my ($author, $year) = ($1, $2);

        
        my $aname = $authors{$author};
        if (!defined $aname) {
            next;
            #print STDERR "Author $author not found! Add to AUTHORS.\n";
            #exit 1;
        }

        $orig_author = $aname;
        $orig_date = $3;
        $orig_date =~ s/,//;

        $author = $aname;

        if (!exists $fauthors{$author}) {
            $fauthors{$author} = [$year, $year];
        } else {
            my ($syear, $eyear) = @{$fauthors{$author}};
            $fauthors{$author} = [min($year, $syear), max($year, $eyear)];
        }
    }

    if (!defined $orig_author) {
        print STDERR "No recognized authorship data found for $file.\n";
        next;
    }

    open FIN, "<$file" or die "Can't open file $file";
    my @lines = <FIN>;
    close FIN;

    my @clines;
    for my $author (sort keys %fauthors) {
        my ($syear, $eyear) = @{$fauthors{$author}};
        my $df = "$syear-$eyear";
        if ($syear == $eyear) {
            $df = $syear;
        }

        my $cline = " *  Copyright (c) $df $author. All rights reserved.\n";
        push @clines, $cline;
    }
    @clines = (
        <<END,
/*
 *  $file
 *  openc2e
 *
 *  Created by $orig_author on $orig_date.
END
        @clines,
        <<END,
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */
END
    );

    splice @lines, 0, 0, @clines;
    rename $file, "$file.bak" or die "Can't rename $file to $file.bak: $!";
    open FILE, ">$file" or die "Can't open $file for writing: $!";
    print FILE @lines or die "Can't write to $file: $!"; 
    close FILE;
}

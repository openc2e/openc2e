#!/usr/bin/perl
use strict;
use warnings;
use Test::Harness qw(&runtests $verbose);

my @tests = glob("unittests/*.cos");

for my $test (@tests) {
    my $wrapper = $test;
    $wrapper =~ s/cos$/pl/;

    open FILE, ">$wrapper";
    print FILE <<EOF;
#!/usr/bin/perl
exec("./openc2e", "-b", "$test");
EOF
    close FILE;
    chmod 0700, $wrapper;

    $test = $wrapper
}

$verbose = 0;
runtests(@tests);
map { unlink } @tests;

#!/usr/bin/perl
# unit testing for caos script, fuzzie, 06/06/04

open(TESTS, './openc2e unittests/ 2>> /dev/null|');

$pass = 0; $fail = 0;
while (<TESTS>) {
  if (/^PASS$/) { $pass++; next; }
  if (/^FAIL$/) { $fail++; next; }
  if (/^TEST: /) {
    $oldtest = $test;
    $test = $_;
    $test =~ s/\n//;
    $test =~ s/TEST: //;
    next if ($pass == 0) && ($fail == 0);
    print "test $oldtest: $pass passed, $fail failed\n";
    $pass = 0; $fail = 0;
    next;
  }
#  print $_;
}
if (($pass != 0) || ($fail != 0)) {
  print "test $test: $pass passed, $fail failed\n";
}
close TESTS;

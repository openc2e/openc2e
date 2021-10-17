#! perl -w
# cosmerger.pl
# Take a cos file, strip comments and debug statements, 
# and output the installation and the script sections
# separately.

open(ISCR, ">>iscr.work");
open(SCRP, ">>scrp.work");

$state = 0;

while (<>)
{
 # trim white space
 s/^\s*(.*?)\s*$/$1/; 

 # ignore comments, debug statements, and iscr
 next if (index($_,"*") == 0);
 next if (index($_,"dbg:") == 0);
 next if ($_ eq "iscr");

 # stop when we reach rscr
 last if ($_ eq "rscr");

 if ($state == 0)
 {
  # endm finishes the iscr bit
  if ($_ eq "endm")
  {
    $state = 1;
    next;
  }
  # as does the first scrp
  if (index($_, "scrp") == 0)
  {
    $state = 1;
  }
 }
 
 if ($state == 0)
 {
  print ISCR $_, "\n";
 }
 else
 {
  print SCRP $+, "\n";
 }
 
}


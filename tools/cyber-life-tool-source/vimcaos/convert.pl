#! /usr/bin/perl -w

#! Feed me a caos help html file made with DBG: HTML, and I'll
#! feed you caos.auto.vim, part of a syntax highlighting file
#! for displaying CAOS in vim.

#! Any additions or updates to francis.irving@creaturelabs.com

$name = "";
$type = "";

while(<>)
{
	chop;
	if (s/<span class=\"command\">(.+)<\/span>/$1/)
	{
		$name = $_;
	}
	if (s/<span class=\"vartype\">\((.+)\)<\/span> $/$1/)
	{
		$type = $_;
		if ($name eq "")
		{
			die "Got command twice"
		}
		if ($type eq "command")
		{
			$type = "caosCommand";
		}
		elsif ($type eq "string")
		{
			$type = "caosStringRV";
		}
		elsif ($type eq "variable")
		{
			$type = "caosVariable";
		}
		elsif ($type eq "float")
		{
			$type = "caosFloatRV";
		}
		elsif ($type eq "integer")
		{
			$type = "caosIntegerRV";
		}
		elsif ($type eq "agent")
		{
			$type = "caosAgent";
		}
		else
		{
			die "unknown type ", $name, "=", $type;
		}
		if ($name =~ m/ /)
		{
			print "syn match $type \"\\<$name\\>\"\n";			
		}
		elsif ($name ne "ovxx" and $name ne "vaxx" and $name ne "mvxx")
		{
			print "syn keyword $type $name \n";
		}
		$name = "";
	}
}

for ($a = 0; $a < 100; ++$a)
{
	printf "syn keyword caosVariable va%02d\n", $a;
	printf "syn keyword caosVariable ov%02d\n", $a;
	printf "syn keyword caosVariable mv%02d\n", $a;
}


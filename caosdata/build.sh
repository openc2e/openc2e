#!/bin/sh

./makedocs.pl
cp caosdata.cpp ..
cd hash
make
cd ..
hash/generatehex b prehash_cmds.txt > prehash_cmds_b.txt
hash/generatehex b prehash_funcs.txt > prehash_funcs_b.txt
hash/generatehex l prehash_cmds.txt > prehash_cmds_l.txt
hash/generatehex l prehash_funcs.txt > prehash_funcs_l.txt

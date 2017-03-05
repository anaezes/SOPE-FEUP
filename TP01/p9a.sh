#!/bin/bash

rm -r -f $1
rm -r -f "$1.o"

STR1=$1
STR2="$STR1.c"

gcc -Wall $STR2 -o $STR1

if [ $? != 0 ]; then
	echo COMPILATION ERROR
else
	./$STR1
fi

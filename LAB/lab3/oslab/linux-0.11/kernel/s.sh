#!/bin/bash

for filename in $@
do
	if [ -f $filename ]
	then
#	echo "$filename"
#	sed -n  's/\(fprintk\)\(.*\)jiffies\/HZ\(.*\)/\1\2jiffies\3/p' $filename
	sed -n 's/\(fprintk\)/&/p' $filename
	fi
done			

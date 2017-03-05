#!/bin/bash

COUNTER="1"

while [ $COUNTER -le $1 ]; do
	
	if [ $COUNTER -lt 10 ]; then
		mkdir prob0$COUNTER
	else
		mkdir prob$COUNTER
	fi

	COUNTER=$(($COUNTER+1))	
done
#!/bin/bash

############################################
# Declare Variables:                       #
# $1 for test times                        #
# $2 for p2 location                       #
# $3 for query location (file)             #
# $4 for p2 output location                #
# $5 for lemondb output location           #
############################################
i=0
while [ $i -lt $1 ]; do
	echo -e "\ntest ${i}"
	time $2 < $3 > $4
	diff -Bw $5 $4
	echo -e "\ntest ${i} END"
	i=$(($i+1))
done

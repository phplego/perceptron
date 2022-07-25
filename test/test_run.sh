#!/bin/bash

if [[ $1 -ne '' ]]
then
    # if script argument is not empty, use it
    TESTNAME=$1
else
    # ask user otherways
    echo "Enter test name (number or string):"
    read TESTNAME
fi 


file="test_$TESTNAME"

# build
g++ "$file.cpp" -o $file.out -I../lib -pthread

# if no error then run binary
if [[ $? -eq 0 ]]
then
    ./$file.out $2 $3 $4
fi



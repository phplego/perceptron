#!/bin/bash

g++ `pkg-config --cflags gtk+-3.0` gui.cpp `pkg-config --libs gtk+-3.0` -o gui.out -I./lib


if [[ $? -eq 0 ]]
then
    ./gui.out $1 $2
fi



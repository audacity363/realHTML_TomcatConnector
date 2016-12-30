#!/bin/bash

is_aix=$(uname)

#1 = AIX
#2 = Opensuse
ostype=-1

if [is_aix == "AIX"] then
    ostype=1
elif [ -a /etc/os-release ]
then
    ostype=2
else
    echo "Unkown OS. Please edit the Makefile by yourself"
    exit
fi

echo "OSType: $ostype"

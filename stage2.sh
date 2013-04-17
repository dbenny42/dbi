#!/bin/bash

# ensure you pass the correct number of arguments.
if [ $# -ne 2 ]
then
    echo "usage: ./stage2.sh <query_file> <config_file>"
fi

# build the executable if it's not yet there.
if [ ! -e selconds ]
then
    make
fi

./selconds $1 $2

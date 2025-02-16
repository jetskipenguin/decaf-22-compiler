#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 <filepath>"
    exit 1
fi

filepath=$1

if [ ! -f "$filepath" ]; then
    echo "Error: File '$filepath' does not exist."
    exit 1
fi

./decaf-22-compiler "$filepath" > /dev/stdout

#!/bin/bash

# Set output binary name
OUTPUT="decaf-22-compiler"

# Find all .cpp files in the current directory
CPP_FILES=$(find ../src -type f -name "*.cpp")

# Check if any .cpp files were found
if [ -z "$CPP_FILES" ]; then
    echo "No .cpp files found. Exiting."
    exit 1
fi

# Compile using g++
echo "Compiling..."
g++ -Wall -Wextra -std=c++11 $CPP_FILES -o $OUTPUT

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Output: $OUTPUT"
else
    echo "Compilation failed."
fi

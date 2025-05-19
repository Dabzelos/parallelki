#!/bin/bash

DIRS="1_task 2_task filters tests"

files=$(find $DIRS -type f \( -name "*.c" -o -name "*.h" \))

clang-format --Werror --verbose -i $files 

echo "Files formatted successfully."
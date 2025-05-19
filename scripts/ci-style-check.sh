#!/bin/bash

DIRS="1_task 2_task filters tests"

files=$(find $DIRS -type f \( -name "*.c" -o -name "*.h" \))

clang-format --Werror --dry-run -i $files
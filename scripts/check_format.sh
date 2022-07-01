#!/bin/bash

for f in `find src include -name "*.cpp" -o -name "*.hpp"`; do
    clang-format --dry-run -Werror $f
done
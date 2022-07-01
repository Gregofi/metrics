#!/bin/bash

failed=0
for f in `find src include -name "*.cpp" -o -name "*.hpp"`; do
    clang-format --dry-run -Werror $f
    if [[ $? -ne 0 ]]; then
        failed=1
    fi
done

exit ${failed}

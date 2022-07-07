#!/bin/bash

cd build
ninja
cd Tests
something_failed=0
for test in *; do
    if [ -f "$test" ] && [ -x "$test" ]; then
        echo -e "——— Testing \e[1m$test\e[m ———"
        ./$test
        if [ $? -eq 1 ]; then
            echo -e "\033[41m FAIL \033[m $test"
            something_failed=1
        else
            echo -e "\033[42m PASS \033[m $test"
        fi
    fi
done

exit $something_failed

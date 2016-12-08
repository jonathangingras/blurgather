#!/bin/sh

RESULT=0
NTESTS=0

TESTS=$(find $(pwd) -name "*_test")

for test_ in $TESTS
do
    ${test_} --no-fork ${OUTPUT}

    RESULT=${RESULT}+${?}
    NTESTS=${NTESTS}+1
done

RESULT=$(echo $RESULT | bc)
NTESTS=$(echo $NTESTS | bc)

function bold() {
    echo "\033[1m${1}\033[0m"
}

function greenbold() {
    echo "\033[1;32m${1}\033[0m"
}

function redbold() {
    echo "\033[1;31m${1}\033[0m"
}

function format_result() {
    if [ $1 -eq 0 ]
    then
        echo $(greenbold PASSED)
    else
        echo $(redbold FAILED)
    fi
}

echo
echo --------------------------
echo ' TEST SUITES RAN :' $(bold $NTESTS)
echo ' GLOBAL RESULT   :' $(format_result $RESULT)
echo --------------------------

exit $RESULT

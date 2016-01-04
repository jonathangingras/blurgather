#!/bin/sh

RESULT=0

for test_ in build/bin/*_test; do
  ${test_} --no-fork
  RESULT=${RESULT}+${?}
done

exit $(echo $RESULT | bc)

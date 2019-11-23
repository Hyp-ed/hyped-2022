#!/bin/bash

set -e

command -v lcov > /dev/null 2>&1 || {
    echo >&2 "PLEASE INSTALL LCOV TO CALCULATE COVERAGE"
    exit 1
}

command -v genhtml > /dev/null 2>&1 || {
    echo >&2 "PLEASE INSTALL LCOV TO CALCULATE COVERAGE"
    exit 1
}

mkdir -p test/coverage/covdata
echo "Calculating Initial Coverage"
lcov -q -i --directory  bin/debug -b . --capture --no-external --output-file ./test/coverage/covdata/hyped.base.covdata 
lcov -q --remove ./test/coverage/covdata/hyped.base.covdata `pwd`/lib/\* `pwd`/\*.test.cpp -o ./test/coverage/covdata/hyped.base.covdata
echo "Calculating Test Coverage"
lcov -q --directory bin/debug -b . --capture --no-external --output-file ./test/coverage/covdata/hyped.test.covdata
lcov -q --remove ./test/coverage/covdata/hyped.test.covdata `pwd`/lib/\* `pwd`/\*.test.cpp -o ./test/coverage/covdata/hyped.test.covdata
echo "Combining Coverage Stats"
lcov -q -a ./test/coverage/covdata/hyped.base.covdata -a ./test/coverage/covdata/hyped.test.covdata -o ./test/coverage/covdata/hyped.covdata
if [ -z $CI ];
then
  echo "Generating HTML Report"
  genhtml --output-directory ./test/coverage/coverage_html ./test/coverage/covdata/hyped.covdata --quiet
  echo "HTML Report saved at test/coverage/coverage_html"
fi
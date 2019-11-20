mkdir -p test/covdata
lcov -q -i --directory  bin/debug -b . --capture --no-external --output-file ./test/covdata/hyped.base.covdata 
lcov -q --remove ./test/covdata/hyped.base.covdata `pwd`/lib/\* `pwd`/\*.test.cpp -o ./test/covdata/hyped.base.covdata
lcov -q --directory bin/debug -b . --capture --no-external --output-file ./test/covdata/hyped.test.covdata
lcov -q --remove ./test/covdata/hyped.test.covdata `pwd`/lib/\* `pwd`/\*.test.cpp -o ./test/covdata/hyped.test.covdata
lcov -q -a ./test/covdata/hyped.base.covdata -a ./test/covdata/hyped.test.covdata -o ./test/covdata/hyped.covdata
genhtml --output-directory ./test/coverage_html ./test/covdata/hyped.covdata --quiet
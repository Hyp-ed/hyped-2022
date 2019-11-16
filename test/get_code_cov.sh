mkdir -p test/covdata
lcov -i --directory bin -b . --capture --no-external --output-file ./test/covdata/hyped.base.covdata
lcov --remove ./test/covdata/hyped.base.covdata `pwd`/lib/\* `pwd`/\*.test.cpp -o ./test/covdata/hyped.base.covdata
lcov --directory bin -b . --capture --no-external --output-file ./test/covdata/hyped.test.covdata
lcov --remove ./test/covdata/hyped.test.covdata `pwd`/lib/\* `pwd`/\*.test.cpp -o ./test/covdata/hyped.test.covdata
lcov -a ./test/covdata/hyped.base.covdata -a ./test/covdata/hyped.test.covdata -o ./test/covdata/hyped.covdata
genhtml --output-directory ./test/coverage_html ./test/covdata/hyped.covdata
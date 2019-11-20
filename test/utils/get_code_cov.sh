if [ -z `which lcov` ]; then
  echo Please install lcov to calculate coverage
  exit 1
fi

if ![ genhtml -v given-command > /dev/null 2>&1 ]; then
  echo Please install lcov to calculate coverage
  exit 1
fi

mkdir -p test/coverage/covdata
echo "Calculating Initial Coverage"
lcov -q -i --directory  bin/debug -b . --capture --no-external --output-file ./test/coverage/covdata/hyped.base.covdata 
lcov -q --remove ./test/coverage/covdata/hyped.base.covdata `pwd`/lib/\* `pwd`/\*.test.cpp -o ./test/coverage/covdata/hyped.base.covdata
echo "Calculating Test Coverage"
lcov -q --directory bin/debug -b . --capture --no-external --output-file ./test/coverage/covdata/hyped.test.covdata
lcov -q --remove ./test/coverage/covdata/hyped.test.covdata `pwd`/lib/\* `pwd`/\*.test.cpp -o ./test/coverage/covdata/hyped.test.covdata
echo "Combining Coverage Stats"
lcov -q -a ./test/coverage/covdata/hyped.base.covdata -a ./test/coverage/covdata/hyped.test.covdata -o ./test/coverage/covdata/hyped.covdata
echo "Generating HTML Report"s
genhtml --output-directory ./test/coverage/coverage_html ./test/coverage/covdata/hyped.covdata --quiet
echo "HTML Report saved at test/coverage/coverage_html"

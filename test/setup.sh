cd googletest && mkdir build && cd build
cmake -DCMAKE_CXX_COMPILER="c++" -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++" --silent ..
pwd
cd build && make --silent && make install --silent

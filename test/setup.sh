mkdir googletest
tar -xf gtest.tar.gz -C googletest --strip-components 1
cd googletest && mkdir build
cd build && cmake -DCMAKE_CXX_COMPILER="c++" -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++"  ..
make && make install

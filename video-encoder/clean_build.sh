rm out*
rm encoded*
rm decoded*
mkdir build/
cd build/
cmake -DCMAKE_CXX_COMPILER=/usr/bin/c++ ..
cd ..
make -C build/

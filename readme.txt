compile:
1. cd build
2. cmake ../
3. make

cross compile:
1. cd build
2. cmake ../ -DCMAKE_C_COMPILER=arm-linux-gcc -DCMAKE_CXX_COMPILER=arm-linux-g++
3. make

cd ../makefile
rm -rf CMakeFiles
rm -rf CMakeCache.txt
rm -rf cmake_install.cmake
rm -rf Makefile

cmake . -DLIB_D2D_SHARED=ON &&make

cd ../makefile
rm -rf CMakeFiles
rm -rf CMakeCache.txt
rm -rf cmake_install.cmake
rm -rf Makefile

cmake . -DRRC_DST=ON -DRRC_RLC_UT=OFF -DLIB_D2D_SHARED=ON &&make

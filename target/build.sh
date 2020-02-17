cp ../platform/osp_lib/libosp.so .
cp ../platform/osp_lib/libosp.so /lib
cp libD2D_protocol_shared.so /lib
rm -rf main
rm -rf ospmem
#arm-linux-gnueabihf-gcc -o main -lD2D_protocol_shared -losp -pthread -lm -lrt -ldl
gcc -o main -lD2D_protocol_shared -losp -pthread -lm -lrt -ldl
ls -l 

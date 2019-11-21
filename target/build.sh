
cp libosp.so /lib
cp libD2D_protocol_shared.so /lib

gcc -o main -lD2D_protocol_shared -losp -pthread -lm -lrt -ldl

all: mwpm

mwpm: mwpm.cpp ./blossom5-v2.03.src/libblossom5.a
	g++  -g -I./blossom5-v2.03.src/ mwpm.cpp  ./blossom5-v2.03.src/libblossom5.a -lrt  -o mwpm

./blossom5-v2.03.src/libblossom5.a: ./blossom5-v2.03.src/*.cpp
	cd ./blossom5-v2.03.src &&  make

clean: 
	rm -f ./mwpm  ./mwpm.exe
	cd ./blossom5-v2.03.src && make clean
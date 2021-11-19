all: sssp_serial sssp_parallel

sssp_serial: util.o src/sssp_serial.cpp
	g++ -o sssp_serial src/sssp_serial.cpp util.o

sssp_parallel: util.o src/sssp_parallel.cpp
	mpic++ -o sssp_parallel src/sssp_parallel.cpp util.o

util.o: src/util.c src/util.h
	g++ -c src/util.c
    

.PHONY: clean
clean:
	rm util.o sssp_serial sssp_parallel

CPP=CC
CFLAGS=-lm
OPTFLAGS=
COPTFLAGS=-Ofast -ffast-math -march=native -ftree-vectorize -funroll-loops -mtune=native -fno-math-errno -falign-loops=32
MPIFLAGS=-DMPI
DEBUGFLAGS=-g -pg

NVCC=nvcc
NVCCFLAGS=-DCUDA -Xptxas -O3 -arch=sm_80 -use_fast_math


all: base blocked openmp

base: main.cpp smith-waterman.cpp
	$(CPP) $^ -o $@ $(CFLAGS) $(OPTFLAGS)

cuda: main.cpp cuda.cu
	$(NVCC) $^ -o $@ $(NVCCFLAGS)

blocked: main.cpp smith_waterman_blocked.cpp
	$(CPP) $^ -o $@ $(CFLAGS) $(COPTFLAGS)

openmp: main.cpp smith_waterman_openmp.cpp
	$(CPP) $^ -o $@ $(CFLAGS) $(COPTFLAGS) -fopenmp

.PHONY: clean

clean:
	rm -f *.out
	rm -f *.o
	rm -f *.gif
	rm -f base
	rm -f openmp
	rm -f cuda
	rm -f blocked
CPP=CC
CFLAGS=-lm
OPTFLAGS=-Ofast -ffast-math -march=native -ftree-vectorize -funroll-loops -mtune=native -fno-math-errno -falign-loops=32
COPTFLAGS=-Ofast -ffast-math -march=native -ftree-vectorize -funroll-loops -mtune=native -fno-math-errno -falign-loops=32
MPIFLAGS=-DMPI
DEBUGFLAGS=-g -pg

NVCC=nvcc
NVCCFLAGS=-DCUDA -Xptxas -O3 -arch=sm_80 -use_fast_math


base: main.cpp smith-waterman.cpp
	$(CPP) $^ -o $@ $(CFLAGS) $(OPTFLAGS)

cuda: main.cpp cuda.cu
	$(NVCC) $^ -o $@ $(NVCCFLAGS)

openmp: main.cpp smith_waterman_openmp.cpp
	$(CPP) $^ -o $@ $(CFLAGS) $(COPTFLAGS)

.PHONY: clean

clean:
	rm -f *.out
	rm -f *.o
	rm -f *.gif
	rm -f base
	rm -f openmp
	rm -f cuda
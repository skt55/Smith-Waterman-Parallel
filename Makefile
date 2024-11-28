CPP=CC
CFLAGS=-lm
OPTFLAGS=
COPTFLAGS=-Ofast -ffast-math -march=native -ftree-vectorize -funroll-loops -mtune=native -fno-math-errno -falign-loops=32
MPIFLAGS=-DMPI
DEBUGFLAGS=-g -pg 

NVCC=nvcc
NVCCFLAGS=-DCUDA -Xptxas -O3 -arch=sm_80 -use_fast_math

BUILD_DIR=build

BUILD_DIR=build

# Ensure the build directory exists before compiling
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Define the targets without the need for 'build'
base: $(BUILD_DIR)/base
blocked: $(BUILD_DIR)/blocked
openmp: $(BUILD_DIR)/openmp
cuda: $(BUILD_DIR)/cuda
kernel: $(BUILD_DIR)/kernel

# The build rules for each target
$(BUILD_DIR)/base: main.cpp smith-waterman.cpp | $(BUILD_DIR)
	$(CPP) $^ -o $@ $(CFLAGS) $(OPTFLAGS)

$(BUILD_DIR)/cuda: main.cpp cuda.cu | $(BUILD_DIR)
	$(NVCC) $^ -o $@ $(NVCCFLAGS)

$(BUILD_DIR)/blocked: main.cpp smith_waterman_blocked.cpp | $(BUILD_DIR)
	$(CPP) $^ -o $@ $(CFLAGS) $(COPTFLAGS)

$(BUILD_DIR)/openmp: main.cpp smith_waterman_openmp.cpp | $(BUILD_DIR)
	$(CPP) $^ -o $@ $(CFLAGS) $(COPTFLAGS)

$(BUILD_DIR)/kernel: main.cpp smith_waterman_kernel.cpp | $(BUILD_DIR)
	$(CPP) $^ -o $@ $(CFLAGS) $(COPTFLAGS)

# The all target
all: base blocked openmp cuda kernel

# Clean command
.PHONY: clean



clean:
	rm -rf $(BUILD_DIR)/*.o
	rm -rf $(BUILD_DIR)/*.out
	rm -rf $(BUILD_DIR)/*.gif


# Compiler settings
CXX = g++
NVCC = nvcc
CXXFLAGS = -Wall -g
OPENMP_FLAGS = -fopenmp
CUDA_FLAGS = -lcudart -lcuda

# Base target
TARGET_BASE = smith_waterman
SOURCES_BASE = main.cpp smith-waterman.cpp
OBJECTS_BASE = $(SOURCES_BASE:.cpp=.o)

# OpenMP target
TARGET_OPENMP = smith_waterman_openmp
SOURCES_OPENMP = main.cpp smith-waterman-openmp.cpp
OBJECTS_OPENMP = $(SOURCES_OPENMP:.cpp=.o)

# CUDA target
TARGET_CUDA = smith_waterman_cuda
SOURCES_CUDA = main.cpp smith-waterman-cuda.cu
OBJECTS_CUDA = $(SOURCES_CUDA:.cu=.o)

# Base target rules
base: $(TARGET_BASE)

$(TARGET_BASE): $(OBJECTS_BASE)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# OpenMP target rules
openmp: $(TARGET_OPENMP)

$(TARGET_OPENMP): $(OBJECTS_OPENMP)
	$(CXX) $(CXXFLAGS) $(OPENMP_FLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OPENMP_FLAGS) -c $<

# CUDA target rules
cuda: $(TARGET_CUDA)

$(TARGET_CUDA): $(OBJECTS_CUDA)
	$(NVCC) $(CUDA_FLAGS) -o $@ $^

%.o: %.cu
	$(NVCC) $(CUDA_FLAGS) -c $<

# Clean rule
clean:
	rm -f $(OBJECTS_BASE) $(OBJECTS_OPENMP) $(OBJECTS_CUDA) $(TARGET_BASE) $(TARGET_OPENMP) $(TARGET_CUDA)

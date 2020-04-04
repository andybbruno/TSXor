CXX = g++
CXXFLAGS = -O3 -std=c++17

all: compression beringei comprfile

compression: 
	$(CXX) $(CXXFLAGS) -o test_compression.o test_compression.cpp
beringei:
	$(CXX) $(CXXFLAGS) -o test_beringei.o test_beringei.cpp
comprfile:
	$(CXX) $(CXXFLAGS) -o test_compression_dataset.o test_compression_dataset.cpp
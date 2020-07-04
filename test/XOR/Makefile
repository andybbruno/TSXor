CXX 		= g++
FLAGS 		= -O3 -std=c++17

TARGETS = full xor_compression xor_decompression

all		: $(TARGETS)

full:
	$(CXX) $(FLAGS) full.cpp -o full.o
xor_compression:
	$(CXX) $(FLAGS) xor_compression.cpp -o xor_compression.o
xor_decompression:
	$(CXX) $(FLAGS) xor_decompression.cpp -o xor_decompression.o
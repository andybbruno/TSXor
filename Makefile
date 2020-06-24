CXX 		= g++ 
FLAGS 		= -O3 -std=c++17

TARGETS = test

all		: $(TARGETS)


test:
	$(CXX) $(FLAGS) test.cpp -o test.o
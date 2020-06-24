CXX 		= g++ 
FLAGS 		= -O3 -std=c++17
TESTFOLDER  = test/
UTILFOLDER  = util/

TARGETS = full compression decompression csv_to_bin

all		: $(TARGETS)

full:
	$(CXX) $(FLAGS) $(TESTFOLDER)full.cpp -o $(TESTFOLDER)full.o
compression:
	$(CXX) $(FLAGS) $(TESTFOLDER)compression.cpp -o $(TESTFOLDER)compression.o
decompression:
	$(CXX) $(FLAGS) $(TESTFOLDER)decompression.cpp -o $(TESTFOLDER)decompression.o
csv_to_bin:
	$(CXX) $(FLAGS) $(UTILFOLDER)csv_to_bin.cpp -o $(UTILFOLDER)csv_to_bin.o
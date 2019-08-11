source = fast.cpp range_search.cpp gen_lognormal.cpp gen_data.cpp test_lognorm_190M.cpp insert_sample_0.1.cpp insert_sample_0.01.cpp
objects = 
executables = ./build/range_search ./build/gen_lognormal ./build/fast ./build/gen_data ./build/test_lognorm_190M ./build/insert_sample_0.1 ./build/insert_sample_0.01
CXX = g++
#LANGFLAGS = -std=c++14
LANGFLAGS = -std=c++1y
CXXFLAGS = -Ofast $(LANGFLAGS) -march=core-avx2 -msse -Iinclude -pedantic
LIBS =

all: gen_lognormal gen_data range_search fast test_lognorm_190M insert_sample_0.1 insert_sample_0.01

gen_lognormal:
	$(CXX) $(CXXFLAGS) -o ./build/gen_lognormal ./src/gen_lognormal.cpp $(LIBS)

gen_data:  
	$(CXX) $(CXXFLAGS) -o ./build/gen_data ./src/gen_data.cpp $(LIBS)

range_search:  
	$(CXX) $(CXXFLAGS) -o ./build/range_search ./src/range_search.cpp $(LIBS)

fast:  
	$(CXX) $(CXXFLAGS) -o ./build/fast ./src/fast.cpp $(LIBS)

test_lognorm_190M:
	$(CXX) $(CXXFLAGS) -o ./build/test_lognorm_190M ./src/test_lognorm_190M.cpp $(LIBS)

insert_sample_0.1:
	$(CXX) $(CXXFLAGS) -o ./build/insert_sample_0.1 ./src/insert_sample_0.1.cpp $(LIBS)

insert_sample_0.01:
	$(CXX) $(CXXFLAGS) -o ./build/insert_sample_0.01 ./src/insert_sample_0.01.cpp $(LIBS)


.PHONY: clean
clean:
	-rm -f $(executables) ./build/*.o ./build/*

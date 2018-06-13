EXECS=TestRandomGraphs ReduceGraph ParseReps
OPTFLAGS=-O2 -std=c++11
OPTLOCALFLAGS= -DLOCAL -std=c++11 -O2
DBGFLAGS= -DLOCAL -lubsan -std=c++11 -fsanitize=address -g -fsanitize=undefined -pedantic -D_GLIBCXX_DEBUG -Wall -Wextra -Wfloat-equal -Wlogical-op -Wshadow -Wunused
#CXXFLAGS= -DLOCAL -lubsan -std=c++11 -fsanitize=address -g -fsanitize=undefined -pedantic -D_GLIBCXX_DEBUG -Wall -Wextra -Wfloat-equal -Wlogical-op -Wshadow -Wunused
CXXFLAGS=-DLOCAL -std=c++11 -O2
LDFLAGS += -fsanitize=address -fsanitize=undefined
SOURCES=$(wildcard *.cpp)
DEPENDS=$(patsubst %.cpp,.%.depends,$(SOURCES))
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))

all: $(EXECS)

TestRandomGraphs: TestRandomGraphs.o ReadTxt.o FilesOps.o FlagParser.o CommonGraph.o UQWReviver.o ComputeWReach.o Independent.o UQW.o
	$(CXX) $(LDFLAGS) $^ -o $@

ReduceGraph: ReduceGraph.o ReadTxt.o FilesOps.o FlagParser.o CommonGraph.o UQWReviver.o ComputeWReach.o Independent.o UQW.o
	$(CXX) $(LDFLAGS) $^ -o $@	

ParseReps: ParseReps.o FilesOps.o
	$(CXX) $(LDFLAGS) $^ -o $@	
	
.%.depends: %.cpp
	$(CXX) $(CXXFLAGS) -MM $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(EXECS) $(DEPENDS)

-include $(DEPENDS)
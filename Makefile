CXX = g++
CXXFLAGS = -Wall -O2 -g

main: main.o algos.o graph.o 
	$(CXX) $(CXXFLAGS) -o main main.o algos.o graph.o

morris2014: morris2014.o graph2014.o
	$(CXX) $(CXXFLAGS) -o morris2014 morris2014.o graph2014.o
	
morris2014.o: morris2014.cpp graph2014.h
	$(CXX) $(CXXFLAGS) -c morris2014.cpp

main.o: main.cpp algos.h graph.h
	$(CXX) $(CXXFLAGS) -c main.cpp

algos.o: algos.h

graph.o: graph.h

graph2014.o: graph2014.h

CXX = g++
CXXFLAGS = -Wall -O2 -g

main: main.o algos.o graph.o 
	$(CXX) $(CXXFLAGS) -o main main.o algos.o graph.o

main.o: main.cpp algos.h graph.h
	$(CXX) $(CXXFLAGS) -c main.cpp

algos.o: algos.h

graph.o: graph.h



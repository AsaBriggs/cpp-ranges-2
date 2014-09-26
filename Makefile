CC=g++
CFLAGS=-c -Wall -pedantic --std=c++11 -Os
LDFLAGS=
INCLUDES=range2.h iterator_adapter.h
SOURCES=range2.cpp iterator_adapter.cpp range2_main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=range2

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.cpp $(INCLUDES)
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean 

clean :
	rm -rf $(EXECUTABLE)
	rm -rf *.o

CC=g++
CFLAGS=-c -Wall -pedantic --std=c++11 -Os
LDFLAGS=
INCLUDES=compiler_specifics.h comparisons.h iterator_adapter.h range2.h
SOURCES=compiler_specifics.cpp comparisons.cpp iterator_adapter.cpp range2.cpp range2_main.cpp
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

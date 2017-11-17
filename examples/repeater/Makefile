CC=g++
CFLAGS= -c -std=c++11 -I../../include
LDFLAGS= -static -L../../lib -linject
SOURCES= repeater.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=repeater


all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ../../src/*.o *.o
	rm -f $(EXECUTABLE)


debug: CFLAGS+= -DDEBUG -g
debug: all
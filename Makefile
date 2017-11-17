CC=g++
CFLAGS= -c -std=c++11 -I./include
LDFLAGS= -static
SOURCES= src/injector.cpp src/sock_bridge.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
LIB=lib/libinject.a

all: $(SOURCES) $(LIB)
	
$(LIB): $(OBJECTS) 
	ar rcs $(LIB) $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f src/*.o
	rm -f $(LIB)


debug: CFLAGS+= -DDEBUG -g
debug: all

CC=g++
CFLAGS= -c -std=c++11 -I./include
LDFLAGS= -static
SOURCES= src/injector.cpp src/sock_bridge.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
LIB=lib/libinject.a

all: $(SOURCES) $(LIB)
	
$(LIB): $(OBJECTS) 
	mkdir -p lib
	ar rcs $(LIB) $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f src/*.o
	rm -rf lib


debug: CFLAGS+= -DDEBUG -g
debug: all

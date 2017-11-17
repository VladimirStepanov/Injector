CC=g++
CFLAGS= -c -std=c++11 -I../../include  -I./
LDFLAGS= -static -L../../lib -linject
SOURCES= http_replace.cpp replacer.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=http_replace
TEST_SOURCES =  tests/runner.cpp replacer.cpp 
TEST_OUT = tests/runner.cpp
TEST_IN = tests/replace_test.h
TEST_EXECUTABLE = tests/runner
TEST_OBJECTS=$(TEST_SOURCES:.cpp=.o)
TEST_CFLAGS = -I../

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ../../src/*.o *.o tests/*.o
	rm -f $(EXECUTABLE)
	rm -f $(TEST_EXECUTABLE)
	rm -f $(TEST_OUT)


debug: CFLAGS+= -DDEBUG -g
debug: all

test: test_n $(TEST_SOURCES) $(TEST_EXECUTABLE)

test_n:
	cxxtestgen --error-printer -o $(TEST_OUT) $(TEST_IN) 

$(TEST_EXECUTABLE): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $@ $(LDFLAGS)

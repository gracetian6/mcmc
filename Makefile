COMPILER=g++
IGRAPH_INCLUDE_DIR=/usr/local/include/igraph
IGRAPH_LIB_DIR=/usr/local/lib
CXXFLAGS=-I$(IGRAPH_INCLUDE_DIR) -L$(IGRAPH_LIB_DIR) -ligraph -O3 

OBJECTS=bin/main.o bin/util.o

.PHONY: all clean sample_colorings

all: sample_colorings

clean:
	rm -rf bin/* 

bin/%.o: %.cpp
	$(COMPILER) $(CXXFLAGS) -c -o $@ $<

sample_colorings: $(OBJECTS)
	$(COMPILER) $(CXXFLAGS) $(OBJECTS) -o bin/sample_colorings
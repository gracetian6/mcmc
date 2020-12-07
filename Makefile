COMPILER=gcc
IGRAPH_INCLUDE_DIR=/usr/local/include/igraph
IGRAPH_LIB_DIR=/usr/local/lib
CFLAGS=-I$(IGRAPH_INCLUDE_DIR) -L$(IGRAPH_LIB_DIR) -ligraph

OBJECTS=bin/main.o

.PHONY: all clean sample_colorings

all: sample_colorings

clean:
	rm -rf bin/* 

bin/%.o: %.c
	$(COMPILER) $(CFLAGS) -c -o $@ $<

sample_colorings: $(OBJECTS)
	$(COMPILER) $(CFLAGS) $(OBJECTS) -o bin/sample_colorings
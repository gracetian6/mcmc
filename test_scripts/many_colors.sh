#!/bin/bash

# produce plots, for various values of k (# of colors), on complete graph on 5 vertices

for k in 6 7 8 9
do
	echo "Running test with $k colors\n"
	./bin/sample_colorings --num_vertices 5 --num_colors $k --degree 4 --num_steps 200
done

for ((k=10; k<=20; k+=2))
do
	echo "Running test with $k colors"
	./bin/sample_colorings --num_vertices 5 --num_colors $k --degree 4 --num_steps 200
done
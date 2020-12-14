#!/bin/bash

EPSILON=0.01

for k in 5 6 7
do
	for ((n=4; n<=8; n+=2))
	do
		echo "Running test with $k colors and $n vertices and degree 3\n"
		./bin/sample_colorings --num_vertices=$n --num_colors=$k --degree=3 --stopping_threshold=$EPSILON
	done
done

for k in 6 7
do
	for ((n=5; n<=8; n++))
	do
		echo "Running test with $k colors and $n vertices and degree 4"
		./bin/sample_colorings --num_vertices=$n --num_colors=$k --degree=4 --stopping_threshold=$EPSILON
	done
done
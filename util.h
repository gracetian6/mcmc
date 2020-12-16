#ifndef __UTIL_H
#define __UTIL_H

#include <vector>

#include <igraph.h>

void clear_vector(std::vector<double> *vector);

bool check_valid_coloring(igraph_vector_t *neighbors_enum, uint64_t coloring, int vertex, int color);

double calculate_tv_dist(std::vector<double> vector);

uint64_t find_initial_coloring();

void print_parameters(FILE *fp);

uint64_t shift(int n);

#define GET_NTH_COLOR(x,n)          (x / shift(n)) % num_colors
#define SET_NTH_COLOR(x,n,c)        ((x / shift(n+1)) * shift(n+1)) +\
                                        (c * shift(n)) + (x % shift(n))

#endif
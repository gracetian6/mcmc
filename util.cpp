#include "main.h"
#include "util.h"

void clear_vector(std::vector<double> vector) {
    for (int i = 0; i < num_colorings; i++) vector[i] = 0.0;
}

// check that updating "coloring" by setting "vertex"'s color to "color"
// would remain a valid coloring in "graph"
bool check_valid_coloring(igraph_vector_t *neighbors_vec, uint64_t coloring, int vertex, int color) {
    igraph_vector_t neighbors_vec___;

    igraph_vector_init(&neighbors_vec___, degree);
    igraph_neighbors(&graph, &neighbors_vec___, vertex, IGRAPH_ALL);

    bool valid = true;

    for (int j = 0; j < igraph_vector_size(&neighbors_vec___); j++) {
        int neighbor = VECTOR(neighbors_vec___)[j];
        if (GET_NTH_COLOR(coloring, neighbor) == color) {
            valid = false;
            break;
        }
    }
    igraph_vector_destroy(&neighbors_vec___);

    return valid;
}

// void populate_valid_colors(igraph_t *graph, igraph_vector_t *neighbors, bool *colors) {
//     igraph_neighbors(graph, neighbors, vertex, IGRAPH_ALL);

//     bool valid = true;

//     for (int j = 0; j < igraph_vector_size(neighbors); j++) {
//         int neighbor = VECTOR(*neighbors)[j];
//         if (GET_NTH_COLOR(coloring, neighbor) == color) {
//             valid = false;
//             break;
//         }
//     }
// }


// calculate the total variation distance of a vector from uniform
double calculate_tv_dist(std::vector<double> vector) {
    double dist = 0;

    for (int i = 0; i < num_colorings; i++) {
        double contribution = vector[i] - 1.0/num_colorings;
        dist += contribution > 0 ? contribution : -contribution;
    }

    return dist/2;
}

// find a valid coloring for G and encode it
uint64_t find_initial_coloring() {
    igraph_vector_int_t colors;
    igraph_vector_int_init(&colors, 0);
    igraph_vertex_coloring_greedy(&graph, &colors, IGRAPH_COLORING_GREEDY_COLORED_NEIGHBORS);

    uint64_t coloring = 0;

    for (int vertex = 0; vertex < igraph_vector_int_size(&colors); vertex++) {
        int color = VECTOR(colors)[vertex];
        assert(color <= num_colors);
        coloring = SET_NTH_COLOR(coloring, vertex, color);
    }

    igraph_vector_int_destroy(&colors);

    return coloring;
}

void print_parameters(FILE *fp) {
    // Print parameters
    fprintf(fp, "|V|: %d D = %d\n", num_vertices, degree);
    fprintf(fp, "k = %d\n", num_colors);
}

uint64_t shift(int n) {
    int result = 1;
    for (int i = 0; i < n; i++)
        result *= num_colors;
    return result;
}
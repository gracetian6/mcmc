#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <igraph.h>
#include <stdlib.h>

#define NUM_STEPS               1000
int NUM_VERTICES, NUM_COLORS, DEGREE, COLOR_BITS;

#define GET_NTH_COLOR(x,n)      (unsigned int) ((x >> COLOR_BITS*n) & ((1 << COLOR_BITS) - 1))
#define COLOR_MASK(n)           (uint64_t) ((1 << COLOR_BITS*(n+1)) - (1 << COLOR_BITS*n))
#define SET_NTH_COLOR(x,n,c)    (x & ~COLOR_MASK(n)) | (c << COLOR_BITS*n)
#define ARRAY_SIZE              1 << (NUM_VERTICES*COLOR_BITS)

void clear_dist(double distribution[]) {
    for (int i = 0; i < ARRAY_SIZE; i++) distribution[i] = 0.0;
}

// check that updating "coloring" by setting "vertex"'s color to "color"
// would remain a valid coloring in "graph"
bool check_valid_coloring(igraph_t *graph, uint64_t coloring, int vertex, int color) {
    igraph_vector_t neighbors;
    igraph_vector_init(&neighbors, NUM_VERTICES);
    igraph_neighbors(graph, &neighbors, vertex, IGRAPH_ALL);

    for (int j = 0; j < igraph_vector_size(&neighbors); j++) {
        int neighbor = VECTOR(neighbors)[j];
        if (GET_NTH_COLOR(coloring, neighbor) == color) return false;
    }

    return true;
}

// helper function to print a distribution over colorings
void print_dist(double distribution[], bool valid_colorings[]) {
    for (uint64_t x = 0; x < ARRAY_SIZE; x++) {
        if (valid_colorings[x]) {
            for (int v = 0; v < NUM_VERTICES; v++) {
                printf("%d ", GET_NTH_COLOR(x,v));
            }
            printf(":   %f\n", distribution[x]);
        }
    }
}

// take one "step" on the random walk, or, more precisely, multiply "distribution" by the
// random-walk matrix of the Markov chain and place the result in "new_distribution"
int step(igraph_t *graph, double *distribution, double *new_distribution, bool valid_colorings[]) {
    clear_dist(new_distribution);

    int num_new_colorings = 0;

    // iterate through all possible colorings
    for (uint64_t x = 0; x < ARRAY_SIZE; x++) {
        if (!valid_colorings[x]) continue;

        for (int v = 0; v < NUM_VERTICES; v++) {
            int self_loops = 0;

            for (int c = 0; c < NUM_COLORS; c++) {
                if (check_valid_coloring(graph, x, v, c)) {
                    // this choice of color is a valid coloring of the graph
                    new_distribution[SET_NTH_COLOR(x,v,c)] += (1.0/(NUM_COLORS * NUM_VERTICES)) * distribution[x];

                    if (!valid_colorings[SET_NTH_COLOR(x,v,c)]) {
                        valid_colorings[SET_NTH_COLOR(x,v,c)] = true;
                        num_new_colorings++;
                    }
                } else self_loops++;
            }
            // add properly weighted self-loop
            new_distribution[x] += (((double) self_loops) / (NUM_COLORS * NUM_VERTICES)) * distribution[x];
        }
    }

    return num_new_colorings;
}

// calculate the total variation distance of a distribution from uniform
double calculate_tv_dist(double *distribution, bool valid_colorings[], int num_valid_colorings) {
    double dist = 0;
    for (uint64_t x = 0; x < ARRAY_SIZE; x++) {
        if (valid_colorings[x]) {
            double contribution = distribution[x] - 1.0/num_valid_colorings;
            dist += contribution > 0 ? contribution : -contribution;
        }
    }
    return dist/2;
}

// helper function to swap two arrays of doubles
void swap_arrays(double **a, double **b) {
    double *temp = *a;
    *a = *b;
    *b = temp;
}

// find a valid coloring for G and encode it
uint64_t find_initial_coloring(igraph_t *graph) {
    igraph_vector_int_t colors;
    igraph_vector_int_init(&colors, 0);
    igraph_vertex_coloring_greedy(graph, &colors, IGRAPH_COLORING_GREEDY_COLORED_NEIGHBORS);
    uint64_t coloring = 0;

    for (int vertex = 0; vertex < igraph_vector_int_size(&colors); vertex++) {
        int color = VECTOR(colors)[vertex];
        assert(color <= NUM_COLORS);
        coloring = SET_NTH_COLOR(coloring, vertex, color);
    }

    return coloring;
}

int main( int argc, char *argv[] )  {
    // take in 4 arguments: NUM_VERTICES, NUM_COLORS, DEGREE
    if (argc > 4 || argc <= 1) {
      printf("Error: Run ./bin/sample_colorings NUM_VERTICES NUM_COLORS DEGREE\n");
      return 0;
    }

    NUM_VERTICES = atoi(argv[1]);
    NUM_COLORS = atoi(argv[2]);
    DEGREE = atoi(argv[3]);
    COLOR_BITS = (int) ceil(log((double) NUM_COLORS + 1)/log(2)); //  NUM_COLORS <= 2^COLOR_BITS - 1

    // choose a random undirected graph on NUM_VERTICES vertices, where each edge is included w.p. 1/3
    igraph_t graph;
    igraph_rng_seed(igraph_rng_default(), 42);
    igraph_k_regular_game(&graph, NUM_VERTICES, DEGREE,
                         IGRAPH_UNDIRECTED, IGRAPH_NO_LOOPS);

    igraph_bool_t connected;
    igraph_is_connected(&graph, &connected, 0);
    assert(connected);

    // initialize two large vectors to contain distributions over colorings
    // at each step t, "distribution" represents distribution at step t, and
    // "new_distribution" represents the distribution at step t+1
    // we switch them after taking the step to reuse the allocations
    double *distribution = malloc(sizeof(double) * ARRAY_SIZE);
    clear_dist(distribution);
    double *new_distribution = malloc(sizeof(double) * ARRAY_SIZE);

    // initialize a boolean array representing which numbers encode valid colorings, for convenience
    bool *valid_colorings = malloc(sizeof(bool) * ARRAY_SIZE);
    memset(valid_colorings, 0, sizeof(bool) * ARRAY_SIZE);

    // pick an initial coloring and initialize the distribution to put all probability on this coloring
    uint64_t initial_coloring = find_initial_coloring(&graph);
    distribution[initial_coloring] = 1.0;
    valid_colorings[initial_coloring] = true;

    // tracks how many valid colorings have been seen so far
    int num_valid_colorings = 1;

    // Print parameters
    printf("|V|: %d, k = %d, COLOR_BITS = %d, NUM_STEPS = %d, D = %d\n", NUM_VERTICES, NUM_COLORS, COLOR_BITS, NUM_STEPS, DEGREE);
    printf("Finished initialization!\n===========\n\n");

    // tracks whether we've stop seeing new colorings (and so valid_colorings accurately
    // contains all of the colorings it should)
    bool no_additional_colorings = false;

    // main loop to "advance distribution by one step"
    for (int t = 0; t < NUM_STEPS; t++) {
        // did this step actually discover new valid colorings?
        int new_colorings_found = step(&graph, distribution, new_distribution, valid_colorings);
        if (new_colorings_found) {
            // found additional colorings.
            printf("STEP %d. Found %d additional colorings.\n", t, new_colorings_found);
            num_valid_colorings += new_colorings_found;
        } else {
            // no additional colorings
            no_additional_colorings = true;
            printf("STEP %d. TV-dist: %f.\n", t, calculate_tv_dist(distribution, valid_colorings, num_valid_colorings));
        }
        swap_arrays(&distribution, &new_distribution);
    }
    printf("num_colorings: %d", num_valid_colorings);

    igraph_destroy(&graph);

    return 0;
}

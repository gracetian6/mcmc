#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <igraph.h>
#include <stdlib.h>
#include <getopt.h>

int num_vertices = -1, num_colors = -1, degree = -1;
int color_bits, array_size;
int num_steps = -1;
double stopping_threshold = NAN;

#define GET_NTH_COLOR(x,n)      (unsigned int) ((x >> color_bits*n) & ((1 << color_bits) - 1))
#define COLOR_MASK(n)           (uint64_t) ((1 << color_bits*(n+1)) - (1 << color_bits*n))
#define SET_NTH_COLOR(x,n,c)    (x & ~COLOR_MASK(n)) | (c << color_bits*n)

void clear_dist(double *distribution) {
    for (int i = 0; i < array_size; i++) distribution[i] = 0.0;
}

// check that updating "coloring" by setting "vertex"'s color to "color"
// would remain a valid coloring in "graph"
bool check_valid_coloring(igraph_t *graph, uint64_t coloring, int vertex, int color) {
    igraph_vector_t neighbors;
    igraph_vector_init(&neighbors, num_vertices);
    igraph_neighbors(graph, &neighbors, vertex, IGRAPH_ALL);

    bool valid = true;

    for (int j = 0; j < igraph_vector_size(&neighbors); j++) {
        int neighbor = VECTOR(neighbors)[j];
        if (GET_NTH_COLOR(coloring, neighbor) == color) {
            valid = false;
            break;
        }
    }

    igraph_vector_destroy(&neighbors);

    return valid;
}

// helper function to print a distribution over colorings
void print_dist(double *distribution, bool *valid_colorings) {
    for (uint64_t x = 0; x < array_size; x++) {
        if (valid_colorings[x]) {
            for (int v = 0; v < num_vertices; v++) {
                printf("%d ", GET_NTH_COLOR(x,v));
            }
            printf(":   %f\n", distribution[x]);
        }
    }
}

// take one "step" on the random walk, or, more precisely, multiply "distribution" by the
// random-walk matrix of the Markov chain and place the result in "new_distribution"
int step(igraph_t *graph, double *distribution, double *new_distribution, bool *valid_colorings) {
    clear_dist(new_distribution);

    int num_new_colorings = 0;

    // iterate through all possible colorings
    for (uint64_t x = 0; x < array_size; x++) {
        if (!valid_colorings[x]) continue;

        for (int v = 0; v < num_vertices; v++) {
            int self_loops = 0;

            for (int c = 0; c < num_colors; c++) {
                if (check_valid_coloring(graph, x, v, c)) {
                    // this choice of color is a valid coloring of the graph
                    new_distribution[SET_NTH_COLOR(x,v,c)] += (1.0/(num_colors * num_vertices)) * distribution[x];

                    if (!valid_colorings[SET_NTH_COLOR(x,v,c)]) {
                        valid_colorings[SET_NTH_COLOR(x,v,c)] = true;
                        num_new_colorings++;
                    }
                } else self_loops++;
            }
            // add properly weighted self-loop
            new_distribution[x] += (((double) self_loops) / (num_colors * num_vertices)) * distribution[x];
        }
    }

    return num_new_colorings;
}

// calculate the total variation distance of a distribution from uniform
double calculate_tv_dist(double *distribution, bool *valid_colorings, int num_valid_colorings) {
    double dist = 0;
    for (uint64_t x = 0; x < array_size; x++) {
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
        assert(color <= num_colors);
        coloring = SET_NTH_COLOR(coloring, vertex, color);
    }

    igraph_vector_int_destroy(&colors);

    return coloring;
}

int main(int argc, char *argv[]) {
    // https://stackoverflow.com/questions/1052746/getopt-does-not-parse-optional-arguments-to-parameters/32575314
    int getopt_ret, option_index;
    static struct option long_options[] = {
        {"num_vertices",  required_argument, 0, 'n'},
        {"num_colors",  required_argument, 0, 'k'},
        {"degree",  required_argument, 0, 'd'},
        {"num_steps",  required_argument, 0, 't'},
        {"stopping_threshold", required_argument, 0, 'e'},
        {"seed", required_argument, 0, 's'},
        {0, 0, 0, 0}
    };

    int seed = -1;

    while (true) {
        getopt_ret = getopt_long(argc, argv, "n:k:k:t:e:", long_options,  &option_index);
        if (getopt_ret == -1) break;

        switch(getopt_ret)
        {
            case 0: break;
            case 'n':
                num_vertices = atoi(optarg);
                break;
            case 'k':
                num_colors = atoi(optarg);
                break;
            case 'd':
                degree = atoi(optarg);
                break;
            case 't':
                num_steps = atoi(optarg);
                break;
            case 'e':
                stopping_threshold = atof(optarg);
                break;
            case 's':
                seed = atoi(optarg);
                break;
        }
    }

    if (num_vertices == -1 || num_colors == -1 || num_colors == -1
            || (num_steps == -1 && stopping_threshold == NAN)) {
        printf("usage: sample_colorings [--num_vertices=6] [--num_colors=5] [--degree=3] [--num_steps=1000] [--stopping_threshold 0.001] [--seed=42]\n");
        printf("must pass: --num_vertices, --num_colors, --degree, and at least one of {--num_steps, --stopping_threshold}\n");
        return -1;
    }

    color_bits = (int) ceil(log((double) num_colors)/log(2)); //  num_colors <= 2^color_bits
    array_size = 1 << (color_bits * num_vertices);

    // choose a random undirected graph on num_vertices vertices, where each edge is included w.p. 1/3
    igraph_t graph;
    if (seed != -1) {
        igraph_rng_seed(igraph_rng_default(), seed);
    }
    igraph_k_regular_game(&graph, num_vertices, degree,
                         IGRAPH_UNDIRECTED, IGRAPH_NO_LOOPS);

    igraph_bool_t connected;
    igraph_is_connected(&graph, &connected, 0);
    assert(connected);

    // initialize two large vectors to contain distributions over colorings
    // at each step t, "distribution" represents distribution at step t, and
    // "new_distribution" represents the distribution at step t+1
    // we switch them after taking the step to reuse the allocations
    double *distribution = malloc(sizeof(double) * array_size);
    clear_dist(distribution);
    double *new_distribution = malloc(sizeof(double) * array_size);

    // initialize a boolean array representing which numbers encode valid colorings, for convenience
    bool *valid_colorings = malloc(sizeof(bool) * array_size);
    memset(valid_colorings, 0, sizeof(bool) * array_size);

    // pick an initial coloring and initialize the distribution to put all probability on this coloring
    uint64_t initial_coloring = find_initial_coloring(&graph);
    distribution[initial_coloring] = 1.0;
    valid_colorings[initial_coloring] = true;

    // tracks how many valid colorings have been seen so far
    int num_valid_colorings = 1;

    // initialize File IO
    FILE *fp;
    char fileName[40];
    sprintf(fileName, "data/V%dK%dD%d.csv", num_vertices, num_colors, degree);
    printf("Written into %s\n", fileName);
    fp = fopen(fileName, "w+");

    // Print parameters
    fprintf(fp, "|V|: %d D = %d color_bits = %d array_size = %d\n", num_vertices, degree, color_bits, array_size);
    fprintf(fp, "k = %d\n", num_colors);

    printf("Finished initialization!\n===========\n\n");
    fprintf(fp, "STEP, Additional Colorings, TV-dist\n");

    // tracks whether we've stop seeing new colorings (and so valid_colorings accurately
    // contains all of the colorings it should)
    bool no_additional_colorings = false;

    double tv_dist = 1;
    // main loop to "advance distribution by one step"
    for (int t = 0; t < num_steps || num_steps == -1; t++) {
        printf("Running step %d.\n", t);
        // did this step actually discover new valid colorings?
        int new_colorings_found = step(&graph, distribution, new_distribution, valid_colorings);
        if (new_colorings_found) {
            // found additional colorings.
            fprintf(fp, "%d, %d \n", t, new_colorings_found);
            num_valid_colorings += new_colorings_found;
        } else {
            // no additional colorings
            no_additional_colorings = true;

            double tv_dist = calculate_tv_dist(distribution, valid_colorings, num_valid_colorings);
            fprintf(fp, "%d, ,%f\n", t, tv_dist);
            if (stopping_threshold != NAN && tv_dist <= stopping_threshold) {
                break;
            }
        }
        swap_arrays(&distribution, &new_distribution);
    }
    printf("num_colorings: %d", num_valid_colorings);

    free(distribution);
    free(new_distribution);
    free(valid_colorings);

    igraph_destroy(&graph);

    fclose(fp);
    return 0;
}

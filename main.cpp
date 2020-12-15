#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <igraph.h>
#include <stdlib.h>
#include <getopt.h>

#include <vector>
#include <unordered_map>

int num_vertices = -1, num_colors = -1, degree = -1;
int array_size;
int num_steps = -1;
double stopping_threshold = NAN;

std::vector<double> distribution;
std::vector<double> new_distribution;

std::vector<uint64_t> bitfields;
// std::unordered_map<uint64_t, int> indices;
int *indices;

int num_colorings = 1;

int *adjacent_colorings = NULL;

bool memoize = false;

igraph_t graph;

uint64_t shift(int n) {
    int result = 1;
    for (int i = 0; i < n; i++)
        result *= num_colors;
    return result;
}

#define GET_NTH_COLOR(x,n)          (x / shift(n)) % num_colors
#define SET_NTH_COLOR(x,n,c)        ((x / shift(n+1)) * shift(n+1)) + (c * shift(n)) + (x % shift(n))

// #define GET_NTH_COLOR(x,n)      (unsigned int) ((x >> color_bits*n) & ((1 << color_bits) - 1))
// #define COLOR_MASK(n)           (uint64_t) ((1 << color_bits*(n+1)) - (1 << color_bits*n))
// #define SET_NTH_COLOR(x,n,c)    (x & ~COLOR_MASK(n)) | (c << color_bits*n)

void clear_dist() {
    for (int i = 0; i < num_colorings; i++) new_distribution[i] = 0.0;
}

// check that updating "coloring" by setting "vertex"'s color to "color"
// would remain a valid coloring in "graph"
bool check_valid_coloring(uint64_t coloring, int vertex, int color) {
    igraph_vector_t neighbors;
    igraph_vector_init(&neighbors, num_vertices);
    igraph_neighbors(&graph, &neighbors, vertex, IGRAPH_ALL);

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
// void print_dist() {
//     for (uint64_t x = 0; x < array_size; x++) {
//         if (valid_colorings[x]) {
//             for (int v = 0; v < num_vertices; v++) {
//                 printf("%d ", GET_NTH_COLOR(x,v));
//             }
//             printf(":   %f\n", distribution[x]);
//         }
//     }
// }

// take one "step" on the random walk, or, more precisely, multiply "distribution" by the
// random-walk matrix of the Markov chain and place the result in "new_distribution"
int step() {
    clear_dist();

    int num_new_colorings = 0;

    // iterate through all possible colorings
    for (int i = 0; i < num_colorings; i++) {
        uint64_t x = bitfields[i];

        for (int v = 0; v < num_vertices; v++) {
            int self_loops = 0;

            for (int c = 0; c < num_colors; c++) {
                if (check_valid_coloring(x, v, c)) {
                    // this choice of color is a valid coloring of the graph
                    uint64_t y = SET_NTH_COLOR(x,v,c);

                    if (indices[y] == -1) {
                        indices[y] = num_colorings + (num_new_colorings++);
                        bitfields.push_back(y);
                        distribution.push_back(0);
                        new_distribution.push_back(0);
                    }

                    new_distribution[indices[y]] += (1.0/(num_colors * num_vertices)) * distribution[i];

                } else self_loops++;
            }
            // add properly weighted self-loop

            new_distribution[i] += (((double) self_loops) / (num_colors * num_vertices)) * distribution[i];
        }
    }

    return num_new_colorings;
}

// calculate the total variation distance of a distribution from uniform
double calculate_tv_dist() {
    double dist = 0;

    for (int i = 0; i < num_colorings; i++) {
        double contribution = distribution[i] - 1.0/num_colorings;
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
        {"memoize", optional_argument, 0, 'm'},
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
            case 'm':
                memoize = true;
                break;
        }
    }

    if (num_vertices == -1 || num_colors == -1 || num_colors == -1
            || (num_steps == -1 && stopping_threshold == NAN)) {
        printf("usage: sample_colorings [--num_vertices=6] [--num_colors=5] [--degree=3] [--num_steps=1000] [--stopping_threshold 0.001] [--seed=42]\n");
        printf("must pass: --num_vertices, --num_colors, --degree, and at least one of {--num_steps, --stopping_threshold}\n");
        return -1;
    }

    // choose a random undirected graph on num_vertices vertices, where each edge is included w.p. 1/3
    if (seed != -1) {
        igraph_rng_seed(igraph_rng_default(), seed);
    }
    igraph_k_regular_game(&graph, num_vertices, degree,
                         IGRAPH_UNDIRECTED, IGRAPH_NO_LOOPS);

    igraph_bool_t connected;
    igraph_is_connected(&graph, &connected, IGRAPH_STRONG);
    assert(connected);

    // pick an initial coloring and initialize the distribution to put all probability on this coloring
    uint64_t initial_coloring = find_initial_coloring();

    array_size = shift(num_vertices);
    indices = (int*) malloc(sizeof(int) * array_size);
    for (int i = 0; i < array_size; i++) indices[i] = -1;

    bitfields.push_back(initial_coloring);
    indices[initial_coloring] = 0;
    distribution.push_back(1.0);
    new_distribution.push_back(0.0);

    // initialize File IO
    FILE *fp;
    char fileName[40];
    sprintf(fileName, "data/V%dK%dD%d.csv", num_vertices, num_colors, degree);
    printf("Written into %s\n", fileName);
    fp = fopen(fileName, "w+");
    if (fp == NULL) {
      fprintf(stderr, "Couldn't open %s\n", fileName);
      exit(1);
    }

    // Print parameters
    fprintf(fp, "|V|: %d D = %d array_size = %d\n", num_vertices, degree, array_size);
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
        int new_colorings_found = step();

        if (new_colorings_found) {
            // found additional colorings.
            fprintf(fp, "%d, %d \n", t, new_colorings_found);
            num_colorings += new_colorings_found;
        } else {
            // no additional colorings
            no_additional_colorings = true;

            double tv_dist = calculate_tv_dist();
            fprintf(fp, "%d, ,%f\n", t, tv_dist);
            if (stopping_threshold != NAN && tv_dist <= stopping_threshold) {
                break;
            }
        }
        distribution.swap(new_distribution);
    }
    printf("num_colorings: %d", num_colorings);

    igraph_destroy(&graph);

    fclose(fp);
    return 0;
}

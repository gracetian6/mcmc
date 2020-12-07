#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_STEPS               1000
int NUM_VERTICES, NUM_COLORS, DEGREE, COLOR_BITS;

int main( int argc, char *argv[] )  {


    // take in 4 arguments: NUM_VERTICES, NUM_COLORS, DEGREE
    if (argc > 4 || argc <= 1) {
     printf("Invalid Arguments.\n");
     return 0;
    }

    NUM_VERTICES = atoi(argv[1]);
    NUM_COLORS = atoi(argv[2]);
    DEGREE = atoi(argv[3]);
    COLOR_BITS = (int) ceil(log((double) NUM_COLORS + 1)/log(2)); //  NUM_COLORS <= 2^COLOR_BITS - 1

    printf("|V|: %d, k = %d, COLOR_BITS = %d, NUM_STEPS = %d, D = %d\n", NUM_VERTICES, NUM_COLORS, COLOR_BITS, NUM_STEPS, DEGREE);


    // file I/O
    FILE *fp;
    char fileName[20];
    sprintf(fileName, "V%dK%dD%d.csv", NUM_VERTICES, NUM_COLORS, DEGREE);

    printf("%s\n", fileName);
    // itoa
    fp = fopen(fileName, "w+");
    fprintf(fp, "This is testing for fprintf, ...\n");
    fputs("This is testing for fputs...\n", fp);
    fclose(fp);
}

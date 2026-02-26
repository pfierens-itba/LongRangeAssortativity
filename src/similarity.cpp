#include <igraph/igraph.h>
#include <stdio.h>
#include <stdlib.h> // Include stdlib.h for malloc and free
#include <omp.h>
#include "generalsettings.hpp"
#include "similarity.hpp"

int allocateSimMatrix(double**& simMatrix, igraph_integer_t N) {
    simMatrix = (double**)malloc((N - 1) * sizeof(double*));
    if (simMatrix == NULL) {
        fprintf(stderr, "Memory allocation failed for simMatrix\n");
        return -1;
    }
    for (igraph_integer_t m = 0; m < N - 1; m++) {
        simMatrix[m] = (double*)malloc((N - m - 1) * sizeof(double));
        if (simMatrix[m] == NULL) {
            fprintf(stderr, "Memory allocation failed for simMatrix[%d]\n", (int)m);
            for (igraph_integer_t k = 0; k < m; k++) {
                free(simMatrix[k]);
            }
            free(simMatrix);
            return -1;
        }
    }
    return 0;
}

int freeSimMatrix(double**& simMatrix, igraph_integer_t N) {
    for (igraph_integer_t k = 0; k < N - 1; k++) {
        free(simMatrix[k]);
    }
    free(simMatrix);
    return 0;
}

int buildSimMatrix(double** simMatrix, void* features, void (*cacheSimilarity)(double*, void*, igraph_integer_t, igraph_integer_t), igraph_integer_t N) {
    igraph_integer_t n;
    printf("Building similarity matrix for %zd nodes\n", N);
#pragma omp parallel for num_threads(NO_THREADS)
    for (n = 0; n < N - 1; n++) {
        cacheSimilarity(simMatrix[n] - (n + 1), features, N, n);
    }
    printf("Similarity matrix built\n");
    return 0;
}


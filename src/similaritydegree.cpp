#include <igraph/igraph.h>
#include "generalsettings.hpp"
#include "similarity.hpp"
#include "similaritydegree.hpp"

void cacheSimilarityDegree(double* simRow, void* features, igraph_integer_t N, igraph_integer_t n) {
    igraph_integer_t m;
	igraph_integer_t* degrees = (igraph_integer_t*)features;
    double degn = (double) degrees[n];
    igraph_integer_t* degm = &degrees[n + 1];
    for (m = n + 1; m < N; m++) {
        simRow[m] = degn*((double)(*degm++));
    }
}

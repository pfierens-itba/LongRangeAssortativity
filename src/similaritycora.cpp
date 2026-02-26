#include <igraph/igraph.h>
#include <stdio.h>
#include <intrin.h>
#include <stdint.h>
#include <stdlib.h> // Include stdlib.h for malloc and free
#include <string.h>
#include <immintrin.h>  // AVX2 intrinsics
#include <omp.h>
#include "generalsettings.hpp"
#include "similarity.hpp"
#include "similaritycora.hpp"

static inline double similarity(const featcora a, const featcora b) {
    int i;
	int numerator = 0;
    double sim = 0.0;
    for (i = 0; i < 1433; i++) {
        if (a.f[i] != b.f[i]) {
			numerator++;
        }
    }
	sim = 1.0 - ((double)numerator / 1433.0);
    
    return sim;
}

void cacheSimilarityCora(double* simRow, void* features, igraph_integer_t N, igraph_integer_t n) {
    igraph_integer_t m;
    featcora featureN = ((featcora*) features)[n];
    for (m = n + 1; m < N; m++) {
        featcora featureM = ((featcora*)features)[m];
        simRow[m] = similarity(featureN, featureM);
    }
}

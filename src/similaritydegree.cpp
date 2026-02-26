/*
 * Copyright (c) 2026 Pablo Ignacio Fierens
 *
 * This source code is licensed under the MIT License.
 *
 * IMPORTANT: This file links against the igraph C library, which is
 * licensed under the GNU General Public License (GPL) v2 or later.
 * As a result, any compiled binary or derivative work that includes
 * igraph must be distributed under the terms of the GPL.
 */
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

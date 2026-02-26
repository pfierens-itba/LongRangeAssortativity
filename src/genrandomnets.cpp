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
#include <stdio.h>
#include "genrandomnets.hpp"


int genrandomnet(igraph_t* graph, int type, igraph_integer_t no_of_nodes, igraph_integer_t nrewires, void* vector) {
    igraph_vector_int_t* degrees = nullptr;
	igraph_vector_t* weights = nullptr;

    switch (type) {
    case CONFIG_HEUR:
        degrees = (igraph_vector_int_t*) vector;
        if (igraph_degree_sequence_game(graph, degrees, NULL, IGRAPH_DEGSEQ_FAST_HEUR_SIMPLE) == IGRAPH_SUCCESS) {
             return 0;
        }
        else {
            return -1;
        }
    case CHUNG_LU:
        weights = (igraph_vector_t*) vector;
        if (igraph_chung_lu_game(graph, weights, NULL, false, IGRAPH_CHUNG_LU_ORIGINAL) == IGRAPH_SUCCESS) {
            return 0;
        }
        else {
            return -1;
        }
    case REWIRE:
		igraph_rewiring_stats_t stats;
        if (igraph_rewire(graph, nrewires, IGRAPH_SIMPLE_SW, &stats) == IGRAPH_SUCCESS) {
            return 0;
        }
        else {
            return -1;
        }
    default:
        printf("Unknown network type\n");
        return -1;
    }

    return 0;
}


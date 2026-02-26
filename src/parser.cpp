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
#include <intrin.h>
#include <stdlib.h> // Include stdlib.h for malloc and free
#include <string.h>
#include "similarity.hpp"
#include "parser.hpp"


int static parseEdges(igraph_t* pgraph, const char* edgelist) {
    FILE* file = nullptr;
    if (fopen_s(&file, edgelist, "r") != 0) {
        printf("Error opening file %s", edgelist);
        return -1;
    }
    igraph_read_graph_edgelist(pgraph, file, 0, IGRAPH_UNDIRECTED);
    fclose(file);

    file = nullptr;

    return 0;
}

int static parseFeat(void* pft, size_t sizeoffeat, igraph_integer_t size, const char* featfile) {
    //const char* filename = "deezer_feat.bin";

    FILE* file = nullptr;
    if (fopen_s(&file, featfile, "rb") != 0) {
        printf("Error opening file %s", featfile);
        return -1;
    }

    size_t read_count = fread(pft, sizeoffeat, size, file);
    if (read_count != size) {
        printf("Error reading file");
        printf("Expected to read %d elements, but read %zu elements\n", (int)size, read_count);
        fclose(file);
        return -1;
    }
    fclose(file);

    file = nullptr;

    return 0;
}

int parse(igraph_t* graph, double**& simMatrix, size_t sizeoffeat, void (*cacheSimilarity)(double*, void*,igraph_integer_t,igraph_integer_t), const char* filename1, const char* filename2) {

    igraph_integer_t N; // igraph_int_t is not a valid type, use igraph_integer_t
    void* features = nullptr;
    igraph_vector_int_t degrees;
    /*igraph_setup();*/

    parseEdges(graph, filename1);
    N = igraph_vcount(graph);
    printf("Number of nodes: %zd\n", N);
    printf("Number of edges: %zd\n", igraph_ecount(graph));

    if (sizeoffeat == 0) {
        printf("Features: Degrees\n");
        igraph_vector_int_init(&degrees, 0);
        igraph_degree(graph, &degrees, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
        features = (void*)VECTOR(degrees);
        if (allocateSimMatrix(simMatrix, N) != 0) {
            igraph_vector_int_destroy(&degrees);
            igraph_destroy(graph);
            return -1;
        }
        buildSimMatrix(simMatrix, features, cacheSimilarity, N);
        igraph_vector_int_destroy(&degrees);
    }
    else {
        printf("Features: From file %s\n", filename2);
        features = calloc(N, sizeoffeat);
        if (features == NULL) {
            fprintf(stderr, "Memory allocation failed for features array\n");
            igraph_destroy(graph);
            return -1;
        }
        if (parseFeat(features, sizeoffeat, N, filename2) != 0)
        {
            free(features);
            igraph_destroy(graph);
            return -1;
        }
        if (allocateSimMatrix(simMatrix, N) != 0) {
            free(features);
            igraph_destroy(graph);
            return -1;
        }
        buildSimMatrix(simMatrix, features, cacheSimilarity, N);
        free(features);
    }

    return 0;
}

int parserClean(igraph_t* graph, double**& simMatrix) {

    freeSimMatrix(simMatrix, igraph_vcount(graph));
    igraph_destroy(graph);
    return 0;
}


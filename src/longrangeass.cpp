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
#include <time.h>
#include "parser.hpp"
#include "similaritycora.hpp"
#include "similaritypubmed.hpp"
#include "similaritydeezer.hpp"
#include "similaritydegree.hpp"
#include "assortativitycalc.hpp"

int main(void) {

    igraph_t graph;
    igraph_integer_t N;
    igraph_integer_t M;
    double** simMatrix;
    int nRandom = 1;

    parse(&graph, simMatrix, sizeof(featcora), cacheSimilarityCora, "./processeddatasets/cora_edges.txt", "./processeddatasets/cora_feat.bin");
    N = igraph_vcount(&graph);
    M = igraph_ecount(&graph);
    printf("Cora Network\n");
    printf("Number of nodes: %zd, Number of edges: %zd\n", N, M);
    assortativityShortestPathsRandom(&graph, simMatrix, N, M, nRandom, "cora_simil-1000-redo1.txt");
    parserClean(&graph, simMatrix);

    parse(&graph, simMatrix, sizeof(featpubmed), cacheSimilarityPubmed, "./processeddatasets/pubmed_edges.txt", "./processeddatasets/pubmed_feat.bin");
    N = igraph_vcount(&graph);
    M = igraph_ecount(&graph);
    printf("Pubmed Network\n");
    printf("Number of nodes: %zd, Number of edges: %zd\n", N, M);
    assortativityShortestPathsRandom(&graph, simMatrix, N, M, nRandom, "pubmed_simil-1000-redo1.txt");
    parserClean(&graph, simMatrix);

    parse(&graph, simMatrix, sizeof(featdeezer), cacheSimilarityDeezer, "./processeddatasets/deezer_edges-RO.txt", "./processeddatasets/deezer_feat-RO.bin");
    N = igraph_vcount(&graph);
    M = igraph_ecount(&graph);
    printf("Deezer RO\n");
    printf("Number of nodes: %zd, Number of edges: %zd\n", N, M);
    assortativityShortestPathsRandom(&graph, simMatrix, N, M, nRandom, "deezer_simil-RO-1000-redo1.txt");
    parserClean(&graph, simMatrix);

    parse(&graph, simMatrix, sizeof(featdeezer), cacheSimilarityDeezer, "./processeddatasets/deezer_edges-HU.txt", "./processeddatasets/deezer_feat-HU.bin");
    N = igraph_vcount(&graph);
    M = igraph_ecount(&graph);
    printf("Deezer HU\n");
    printf("Number of nodes: %zd, Number of edges: %zd\n", N, M);
    assortativityShortestPathsRandom(&graph, simMatrix, N, M, nRandom, "deezer_simil-HU-1000-redo1.txt");
    parserClean(&graph, simMatrix);

    parse(&graph, simMatrix, sizeof(featdeezer), cacheSimilarityDeezer, "./processeddatasets/deezer_edges-HR.txt", "./processeddatasets/deezer_feat-HR.bin");
    N = igraph_vcount(&graph);
    M = igraph_ecount(&graph);
    printf("Deezer HR\n");
    printf("Number of nodes: %zd, Number of edges: %zd\n", N, M);
    assortativityShortestPathsRandom(&graph, simMatrix, N, M, nRandom, "deezer_simil-HR-1000-redo1.txt");
    parserClean(&graph, simMatrix);

    return 0;
}

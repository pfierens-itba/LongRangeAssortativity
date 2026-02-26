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

int parse(igraph_t* graph, double**& simMatrix, size_t sizeoffeat, void (*cacheSimilarity)(double*, void*, igraph_integer_t, igraph_integer_t), const char* filename1, const char* filename2);
int parserClean(igraph_t* graph, double**& simMatrix);
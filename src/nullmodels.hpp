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

int computeNullModel(igraph_t* graph, double** simMatrix, igraph_integer_t N, double meanSim[], double meanSqSim[]);
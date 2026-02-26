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

enum randomtypes {
    CONFIG_HEUR,    // 0
    CHUNG_LU,    // 1
    REWIRE,      // 2
};


int genrandomnet(igraph_t* graph, int type, igraph_integer_t no_of_nodes, igraph_integer_t nrewires, void* vector);


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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memorygeneric.hpp"

int  _allocate_dbl_vector(double*& vec, size_t dim1) {
    vec = (double*)malloc(dim1 * sizeof(double));
    if (vec == NULL) {
        return -1;
    }
    return 0;
}

int  _zero_dbl_vector(double* vec, size_t dim1) {
	memset(vec, 0, dim1 * sizeof(double));
    return 0;
}

int  _free_dbl_vector(double*& vec) {
    free(vec);
    return 0;
}


int  _allocate_dbl_matrix(double**& mat, size_t dim1, size_t dim2) {
    mat = (double**)malloc(dim1 * sizeof(double*));
    if (mat == NULL) {
        fprintf(stderr, "Memory allocation failed for mat\n");
        return -1;
    }
    for (size_t k = 0; k < dim1; k++) {
        if (_allocate_dbl_vector(mat[k], dim2) != 0) {
            fprintf(stderr, "Memory allocation failed for mat[%zd]\n", k);
            for (size_t i = 0; i < k; i++) {
                _free_dbl_vector(mat[i]);
            }
            free(mat);
            return -1;
        }
    }
    return 0;
}

int  _zero_dbl_matrix(double** mat, size_t dim1, size_t dim2) {
    for (size_t k = 0; k < dim1; k++) {
        _zero_dbl_vector(mat[k], dim2);
    }
    return 0;
}

int  _free_dbl_matrix(double**& mat, size_t dim1) {
    for (size_t i = 0; i < dim1; i++) {
        _free_dbl_vector(mat[i]);
    }
    free(mat);
    return 0;
}

int  _allocate_dbl_3d_matrix(double***& mat3, size_t dim1, size_t dim2, size_t dim3) {
    mat3 = (double***)malloc(dim1 * sizeof(double**));
    if (mat3 == NULL) {
        return -1;
    }
    for (size_t k = 0; k < dim1; k++) {
        if (_allocate_dbl_matrix(mat3[k], dim2, dim3) != 0) {
            fprintf(stderr, "Memory allocation failed for mat3[%zd]\n", k);
            for (size_t j = 0; j < k; j++) {
                _free_dbl_matrix(mat3[j], dim2);
            }
            free(mat3);
            return -1;
        }
    }
    return 0;
}

int  _zero_dbl_3d_matrix(double*** mat3, size_t dim1, size_t dim2, size_t dim3) {
    for (size_t k = 0; k < dim1; k++) {
        _zero_dbl_matrix(mat3[k], dim2, dim3);
    }
    return 0;
}

int  _free_dbl_3d_matrix(double***& mat3, size_t dim1, size_t dim2) {
    for (size_t k = 0; k < dim1; k++) {
        _free_dbl_matrix(mat3[k], dim2);
    }
    free(mat3);
    return 0;
}

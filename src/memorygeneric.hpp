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

int  _allocate_dbl_vector(double*& vec, size_t dim1);
int  _zero_dbl_vector(double* vec, size_t dim1);
int  _free_dbl_vector(double*& vec);
int  _allocate_dbl_matrix(double**& mat, size_t dim1, size_t dim2);
int  _zero_dbl_matrix(double** mat, size_t dim1, size_t dim2);
int  _free_dbl_matrix(double**& mat, size_t dim1);
int  _allocate_dbl_3d_matrix(double***& mat3, size_t dim1, size_t dim2, size_t dim3);
int  _free_dbl_3d_matrix(double***& mat3, size_t dim1, size_t dim2);
int  _zero_dbl_3d_matrix(double*** mat, size_t dim1, size_t dim2, size_t dim3);


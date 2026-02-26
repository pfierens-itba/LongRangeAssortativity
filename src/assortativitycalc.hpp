int assortativityShortestPaths(igraph_t* graph, double** simMatrix, igraph_integer_t N, bool compute_al, int sample, double ass[], double al[]);
int assortativityShortestPathsRandom(igraph_t* graph, double** simMatrix, igraph_integer_t N, igraph_integer_t M, size_t nRandom, const char* filename);

int allocateSimMatrix(double**& simMatrix, igraph_integer_t N);
int freeSimMatrix(double**& simMatrix, igraph_integer_t N);
int buildSimMatrix(double** simMatrix, void* features, void (*cacheSimilarity)(double*, void*, igraph_integer_t, igraph_integer_t), igraph_integer_t N);

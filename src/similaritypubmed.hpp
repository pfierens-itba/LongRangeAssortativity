#include <igraph/igraph_types.h>
typedef struct {
    double f[500];
} featpubmed;

void cacheSimilarityPubmed(double* simRow, void* features, igraph_integer_t N, igraph_integer_t n);
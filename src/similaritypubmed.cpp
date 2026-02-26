#include <igraph/igraph.h>
#include "generalsettings.hpp"
#include "similarity.hpp"
#include "similaritypubmed.hpp"

static inline double similarity(const featpubmed a, const featpubmed b) {
    int i;
    double sim = 0.0;
	double ddummy = 0.0;
    for (i = 0; i < 500; i++) {
        ddummy = (a.f[i] - b.f[i]);
		sim += ddummy * ddummy;
    }
    sim = 1.0 + sqrt(sim);
    sim = 1.0 / sim;
    return sim;
}

void cacheSimilarityPubmed(double* simRow, void* features, igraph_integer_t N, igraph_integer_t n) {
    igraph_integer_t m;
    featpubmed featureN = ((featpubmed*)features)[n];
    for (m = n + 1; m < N; m++) {
        featpubmed featureM = ((featpubmed*)features)[m];
        simRow[m] = similarity(featureN, featureM);
    }
}

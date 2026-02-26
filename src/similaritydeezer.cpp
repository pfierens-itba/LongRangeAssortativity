#include <igraph/igraph.h>
#include <stdio.h>
#include <intrin.h>
#include <stdint.h>
#include <stdlib.h> // Include stdlib.h for malloc and free
#include <string.h>
#include <immintrin.h>  // AVX2 intrinsics
#include <omp.h>
#include "generalsettings.hpp"
#include "similarity.hpp"
#include "similaritydeezer.hpp"

static inline double similarity(const featdeezer* a, const featdeezer* b) {
    // Jaccard similarity: |A ? B| / |A ? B|
    uint64_t intersection_lo = a->lo & b->lo;
    uint64_t union_lo = a->lo | b->lo;
    uint32_t intersection_hi = a->hi & b->hi;
    uint32_t union_hi = a->hi | b->hi;

    unsigned int numerator = (unsigned int)__popcnt64(intersection_lo) +
        __popcnt(intersection_hi);
    unsigned int denominator = (unsigned int)__popcnt64(union_lo) +
        __popcnt(union_hi);

    // Avoid division by zero; shouldn't happen but adds robustness
    return denominator > 0 ? (double)numerator / (double)denominator : 0.0;
}


// AVX2-optimized batch similarity computation
// Processes 4 feature pairs in parallel using 256-bit vectors
static inline void batch_similarity_avx2_optimized(
    const featdeezer* ref_feature,
    const featdeezer* features,
    igraph_integer_t start,  // Start index
    igraph_integer_t count,  // Number to compute (1-4)
    double* results
) {
    const featdeezer* a_batch[4] = { ref_feature, ref_feature, ref_feature, ref_feature };
    const featdeezer* b_batch[4] = {
        &features[start],
        count > 1 ? &features[start + 1] : &features[start],
        count > 2 ? &features[start + 2] : &features[start],
        count > 3 ? &features[start + 3] : &features[start]
    };

    // Load 4×64-bit lo values into 256-bit vector
    __m256i a_lo = _mm256_setr_epi64x(
        (long long)a_batch[0]->lo,
        (long long)a_batch[1]->lo,
        (long long)a_batch[2]->lo,
        (long long)a_batch[3]->lo
    );

    __m256i b_lo = _mm256_setr_epi64x(
        (long long)b_batch[0]->lo,
        (long long)b_batch[1]->lo,
        (long long)b_batch[2]->lo,
        (long long)b_batch[3]->lo
    );

    // Parallel AND and OR operations on all 4 pairs
    __m256i intersection_lo_vec = _mm256_and_si256(a_lo, b_lo);
    __m256i union_lo_vec = _mm256_or_si256(a_lo, b_lo);

    // Extract results
    uint64_t intersection_lo[4], union_lo[4];
    _mm256_storeu_si256((__m256i*)intersection_lo, intersection_lo_vec);
    _mm256_storeu_si256((__m256i*)union_lo, union_lo_vec);

    // Process 32-bit hi values
    __m256i a_hi = _mm256_setr_epi32(
        a_batch[0]->hi, 0, a_batch[1]->hi, 0,
        a_batch[2]->hi, 0, a_batch[3]->hi, 0
    );

    __m256i b_hi = _mm256_setr_epi32(
        b_batch[0]->hi, 0, b_batch[1]->hi, 0,
        b_batch[2]->hi, 0, b_batch[3]->hi, 0
    );

    __m256i intersection_hi_vec = _mm256_and_si256(a_hi, b_hi);
    __m256i union_hi_vec = _mm256_or_si256(a_hi, b_hi);

    uint32_t intersection_hi[8], union_hi[8];
    _mm256_storeu_si256((__m256i*)intersection_hi, intersection_hi_vec);
    _mm256_storeu_si256((__m256i*)union_hi, union_hi_vec);

    // Compute final similarities
    for (int i = 0; i < count; i++) {
        unsigned int num = (unsigned int)__popcnt64(intersection_lo[i]) +
            __popcnt(intersection_hi[2 * i]);
        unsigned int denom = (unsigned int)__popcnt64(union_lo[i]) +
            __popcnt(union_hi[2 * i]);
        //        results[i] = denom > 0 ? (double)num / (double)denom : 0.0;
        results[i] = denom > 0 ? (double)num / (double)denom : 0.0;
    }
}

// AVX2-optimized cacheSimilarity
void cacheSimilarityDeezer(double* simCache, void* features, igraph_integer_t N, igraph_integer_t n) {
    igraph_integer_t m = n + 1;
    //igraph_integer_t offset = n + 1;
    //igraph_integer_t m = 0;
    double results[4];

    // Process 4 similarities at a time using AVX2
    while (m + 3 < N) {
        batch_similarity_avx2_optimized(&((featdeezer*)features)[n], (featdeezer*)features, m, 4, results);

        simCache[m] = results[0];
        simCache[m + 1] = results[1];
        simCache[m + 2] = results[2];
        simCache[m + 3] = results[3];

        m += 4;
    }

    // Handle remainder (less than 4 left)
    if (m < N) {
        igraph_integer_t remaining = N - m;
        batch_similarity_avx2_optimized(&((featdeezer*)features)[n], (featdeezer*)features, m, remaining, results);

        for (igraph_integer_t i = 0; i < remaining; i++) {
            simCache[m + i] = results[i];
        }
    }

}

/*
int cacheSimilarity(double* simCache, feat* features, igraph_integer_t N, igraph_integer_t n) {
    igraph_integer_t m = n + 1;
    //igraph_integer_t m = 0;

    for (m = 0; m < N; m ++) {
        simCache[m] = similarity(&features[n], &features[m]);
    }

    return 0;
}
*/
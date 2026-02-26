#include <igraph/igraph.h>
#include <stdio.h>
#include <intrin.h>
#include <stdlib.h> // Include stdlib.h for malloc and free
#include <string.h>
#include "generalsettings.hpp"
#include "memorygeneric.hpp"
#include "nullmodels.hpp"
#include "genrandomnets.hpp"
#include "assortativitycalc.hpp"

int assortativityShortestPaths(igraph_t* graph, double** simMatrix, igraph_integer_t N, bool compute_al, int sample,
                               double ass[], double al[]) {
    igraph_adjlist_t adj;
    igraph_vector_int_t* neighbors{};
    igraph_integer_t nneighbors, n, m, w, v, k;
    int maxLength, level, d;
    igraph_integer_t seenCnt, thisLevelCnt, nextLevelCnt;
    igraph_integer_t* level1;
    igraph_integer_t* level2;
    igraph_integer_t* thisLevel;
    igraph_integer_t* nextLevel;
    igraph_integer_t* vector_data;
    double ml[MAX_DEPTH + 1]{ 0.0 };
    double b[MAX_DEPTH + 1]{ 0.0 };
    double aa, mml, dldn, x, simVal;
    double* simRowBase;
    int* seen;
    double** dl = nullptr;
    double* dld = nullptr;
    int currentSeenGen;

    printf("Computation of assortativities - %d\n", sample);
    /**********************************************************************
        MEMORY ALLOCATION
     **********************************************************************/

     // Allocate level memory dynamically
    seen = (int*)malloc((N) * sizeof(int));
    if (seen == NULL) {
        fprintf(stderr, "%d - Memory allocation failed for seen\n", sample);
        return -1;
    }
    level1 = (igraph_integer_t*)malloc((N) * sizeof(igraph_integer_t));
    if (level1 == NULL) {
        fprintf(stderr, "%d - Memory allocation failed for level1\n", sample);
        free(seen);
        return -1;
    }
    level2 = (igraph_integer_t*)malloc((N) * sizeof(igraph_integer_t));
    if (level2 == NULL) {
        fprintf(stderr, "%d - Memory allocation failed for level2\n", sample);
        free(level1);
        free(seen);
        return -1;
    }
    if (compute_al) {
        if (_allocate_dbl_matrix(dl, MAX_DEPTH + 1, N) != 0) {
            fprintf(stderr, "%d - Memory allocation failed for dl\n", sample);
            free(level2);
            free(level1);
            free(seen);
            return -1;
        }
    }

    //igraph_vector_int_init(neighbors, 0);
    igraph_adjlist_init(graph, &adj, IGRAPH_ALL, IGRAPH_NO_LOOPS, IGRAPH_NO_MULTIPLE);
    //printf("%d - Memory allocation done\n", sample);
    /**********************************************************************
        MAIN LOOP OVER NODES
     **********************************************************************/
    if (compute_al) {
		_zero_dbl_matrix(dl, MAX_DEPTH + 1, N);
		_zero_dbl_vector(al, MAX_DEPTH + 1);
    }
    maxLength = 0;
    currentSeenGen = 0;
    memset(seen, 0, N * sizeof(int));
    //I don't care about self-loops
    for (n = 0; n < N; n++) {
        currentSeenGen++;
        if (currentSeenGen == INT_MAX) {
            memset(seen, 0, N * sizeof(int));
            currentSeenGen = 1;
        }
        nextLevel = level1;
        nextLevel[0] = n;
        nextLevelCnt = 1;
        seen[n] = currentSeenGen;
        seenCnt = 1;
        level = 1;
        d = 0;
        simRowBase = simMatrix[n] - (n + 1);
        while ((nextLevelCnt > 0) && (level < MAX_DEPTH) && (seenCnt < N)) {
            thisLevel = nextLevel;
            nextLevel = (thisLevel == level1) ? level2 : level1;
            thisLevelCnt = nextLevelCnt;
            nextLevelCnt = 0;
            if (level > maxLength) {
                maxLength = level;
            }
            if (compute_al) {
                dld = dl[d];
            }
            for (m = 0; m < thisLevelCnt; m++) {
                v = thisLevel[m];
                thisLevel[m] = 0;
                neighbors = igraph_adjlist_get(&adj, v);
                nneighbors = igraph_vector_int_size(neighbors);

                vector_data = VECTOR(*neighbors);
                for (k = 0; k < nneighbors; k++) {
                    //w = VECTOR(*neighbors)[k];
                    w = *vector_data++;
                    //printf("Visiting neighbor %d of node %d at level %d\n", (int)w, (int)v, (int)level);
                    if (seen[w] != currentSeenGen) {
                        seen[w] = currentSeenGen;
                        seenCnt++;
                        nextLevel[nextLevelCnt++] = w;
                        if (w > n) {
                            ml[d] += 2.0;
                            simVal = simRowBase[w];
							simVal += simVal;
                            b[d] += simVal;
                            if (compute_al) {
                                dld[w] += 1.0;
                                dld[n] += 1.0;
                            }
                        }
                        //printf("Node %d added to level %d\n", (int)w, (int)level);
                    }
                }
            }
            d = level++;
            //printf("%d - Node %d, level %d processed, number of nodes added: %d\n", sample, (int)n, (int)level, (int)nextLevelCnt);
        }

        if (seenCnt < N) {
            /*for (m = 0; m < n; m++) {
                if (seen[m] != currentSeenGen) {
                    seen[m] = currentSeenGen;
                }
             }*/
            for (m = n + 1; m < N; m++) {
                if (seen[m] != currentSeenGen){
                    seen[m] = currentSeenGen;
                    //printf("Node %d not reachable from node %d\n", (int)m, (int)n);
					simVal = simRowBase[m];
					simVal += simVal;
                    b[MAX_DEPTH] += simVal;
                    ml[MAX_DEPTH] += 2.0;
                    if (compute_al) {
                        dl[MAX_DEPTH][m] += 1.0;
                        dl[MAX_DEPTH][n] += 1.0;
                    }
                }
            }
        }

//        if (n % 1000 == 0)
//            printf("%d - Node %d processed\n", sample, (int)n);
    }

    /**********************************************************************
        FINAL CALCULATIONS
     **********************************************************************/
    for (level = 0; level < MAX_DEPTH + 1; level++) {
        if (ml[level] > 0) {
            ass[level] = b[level] / ml[level];
        }
        else {
            ass[level] = 0.0;
        }
        if (compute_al) {
            dld = dl[level];
            aa = 0.0;
            mml = 0.0;
            for (n = 0; n < N; n++) {
                dldn = dld[n];
                if (dldn != 0.0) {
                    simRowBase = simMatrix[n] - (n + 1);
                    for (m = n + 1; m < N; m++) {
                        x = dld[m];
                        if (x != 0.0) {
                            x = dldn * x;
                            simVal = simRowBase[m];
                            simVal += simVal;
                            aa += x * simVal;
                            mml += 2.0 * x;
                        }
                    }
                }
            }
            if (mml > 0.0) {
                al[level] = aa / mml;
            }
        }
    }

    /* for (level = 0; level < MAX_DEPTH; level++)
         printf("%d, %f\n", level, ml[level]);
     */
     /**********************************************************************
         FREE MEMORY
      **********************************************************************/

    igraph_adjlist_destroy(&adj);
    //igraph_vector_int_destroy(neighbors);
    if (compute_al) {
        _free_dbl_matrix(dl, MAX_DEPTH + 1);
    }
    free(level2);
    free(level1);
    free(seen);

    printf("Assortativities computed - %d\n", sample);
    return maxLength;
}

int assortativityShortestPathsRandom(igraph_t* graph, double** simMatrix, igraph_integer_t N, igraph_integer_t M, size_t nRandom, const char* filename) {
    int maxLength = -1;
    double** ass = nullptr;
    int level, sample;
    FILE* file = nullptr;
    size_t sampleNo;

    sampleNo = nRandom + 4;
    
    /**********************
       MEMORY ALLOCATION
     **********************/
    printf("Memory allocation\n");
    if (_allocate_dbl_matrix(ass, sampleNo, MAX_DEPTH + 1) != 0) {
        return -1;
    }
    if (fopen_s(&file, filename, "wt+") != 0) {
        fprintf(stderr, "Error opening file");
        _free_dbl_matrix(ass, sampleNo);
        return -1;
    }

    /*******************
        INITIALIZATION
     *******************/
    _zero_dbl_matrix(ass, sampleNo, MAX_DEPTH + 1);
  
    /**************
       MAIN JOB
     **************/
    maxLength = assortativityShortestPaths(graph, simMatrix, N, true, 0, ass[0], ass[1]);
    if (maxLength < 0) {
        fclose(file);
        _free_dbl_matrix(ass, sampleNo);
        return -1;
    }
    if( computeNullModel(graph, simMatrix, N, ass[2], NULL) < 0) {
        fclose(file);
        _free_dbl_matrix(ass, sampleNo);
        return -1;
    }

#pragma omp parallel num_threads(NO_THREADS)
    {
        igraph_t newgraph;
        igraph_copy(&newgraph, graph);
#pragma omp for //schedule(static)
        for (sample = 0; sample < nRandom; sample++) {
            printf("Network: %d/%d - Degree Sequence Preserved\n", (int)sample, (int)nRandom);
            //Rewiring
            genrandomnet(&newgraph, REWIRE, N, 15 * M, NULL);
            if (assortativityShortestPaths(&newgraph, simMatrix, N, false, sample, ass[4 + sample], nullptr) < 0) {
                for (int k = 0; k < MAX_DEPTH + 1; k++) {
                    ass[4 + sample][k] = -2.0;
                }
            }
        }
        igraph_destroy(&newgraph);
    }

    for (sample = 0; sample < sampleNo; sample++) {
        for (level = 0; level < maxLength; level++) {
            fprintf(file, "%.15f\t", ass[sample][level]);
        }
        fprintf(file, "%.15f\n", ass[sample][MAX_DEPTH]);
    }
    
    /**********************
       MEMORY ALLOCATION
     **********************/
    fclose(file);
    _free_dbl_matrix(ass, sampleNo);

    return 0;
}


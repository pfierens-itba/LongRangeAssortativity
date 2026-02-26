#include <igraph/igraph.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include "generalsettings.hpp"
#include "memorygeneric.hpp"

static inline int _free_computeNullModel_resources(
    igraph_vector_int_t* degrees,
    double*& Pk,
    double**& qbar,
    double***& rho,
    double***& P0,
	double**& mean_thread,
    double**& meansq_thread,
    double**& P0Sum_thread,
    igraph_integer_t maxdegree,
	int level) {

    switch (level) {
    case 8:
    case 7:
        _free_dbl_matrix(P0Sum_thread, NO_THREADS);
    case 6:
        _free_dbl_matrix(meansq_thread, NO_THREADS);
    case 5:
        _free_dbl_matrix(mean_thread, NO_THREADS);
    case 4:
        _free_dbl_3d_matrix(P0, MAX_DEPTH + 2, maxdegree + 1);
    case 3:
        _free_dbl_3d_matrix(rho, MAX_DEPTH + 1, maxdegree + 1);
    case 2:
        _free_dbl_matrix(qbar, MAX_DEPTH + 1);
    case 1:
        _free_dbl_vector(Pk);
    case 0:
        igraph_vector_int_destroy(degrees);
    default:
        break;
    }
    return 0;
}

static inline int _allocate_computeNullModel_resources(
	igraph_t* graph,
    igraph_vector_int_t& degrees,
    double*& Pk,
    double**& qbar,
    double***& rho,
    double***& P0,
    double**& mean_thread,
    double**& meansq_thread,
    double**& P0Sum_thread,
    igraph_integer_t& maxdegree,
    int& level) {
    
    level = 0;
    igraph_vector_int_init(&degrees, 0);
    igraph_degree(graph, &degrees, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
    igraph_maxdegree(graph, &maxdegree, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);

    if (_allocate_dbl_vector(Pk, maxdegree + 1) != 0) {
        _free_computeNullModel_resources(&degrees, Pk, qbar, rho, P0, mean_thread, meansq_thread, P0Sum_thread, maxdegree, level);
        return -1;
    }
    level++;
    if (_allocate_dbl_matrix(qbar, MAX_DEPTH + 1, maxdegree + 1) != 0) {
        _free_computeNullModel_resources(&degrees, Pk, qbar, rho, P0, mean_thread, meansq_thread, P0Sum_thread, maxdegree, level);
        return -1;
    }
    level++;
    if (_allocate_dbl_3d_matrix(rho, MAX_DEPTH + 1, maxdegree + 1, maxdegree + 1) != 0) {
        _free_computeNullModel_resources(&degrees, Pk, qbar, rho, P0, mean_thread, meansq_thread, P0Sum_thread, maxdegree, level);
        return -1;
    }
    level++;
    if (_allocate_dbl_3d_matrix(P0, MAX_DEPTH + 2, maxdegree + 1, maxdegree + 1) != 0) {
        _free_computeNullModel_resources(&degrees, Pk, qbar, rho, P0, mean_thread, meansq_thread, P0Sum_thread, maxdegree, level);
        return -1;
    }
    level++;
    if (_allocate_dbl_matrix(mean_thread, NO_THREADS, MAX_DEPTH + 1) != 0) {
        _free_computeNullModel_resources(&degrees, Pk, qbar, rho, P0, mean_thread, meansq_thread, P0Sum_thread, maxdegree, level);
        return -1;
    }
    level++;
    if (_allocate_dbl_matrix(meansq_thread, NO_THREADS, MAX_DEPTH + 1) != 0) {
        _free_computeNullModel_resources(&degrees, Pk, qbar, rho, P0, mean_thread, meansq_thread, P0Sum_thread, maxdegree, level);
        return -1;
    }
    level++;
    if (_allocate_dbl_matrix(P0Sum_thread, NO_THREADS, MAX_DEPTH + 1) != 0) {
        _free_computeNullModel_resources(&degrees, Pk, qbar, rho, P0, mean_thread, meansq_thread, P0Sum_thread, maxdegree, level);
        return -1;
    }
    level++;

    _zero_dbl_vector(Pk, maxdegree + 1);
    _zero_dbl_matrix(qbar, MAX_DEPTH + 1, maxdegree + 1);
    _zero_dbl_3d_matrix(rho, MAX_DEPTH + 1, maxdegree + 1, maxdegree + 1);
    _zero_dbl_3d_matrix(P0, MAX_DEPTH + 2, maxdegree + 1, maxdegree + 1);
    _zero_dbl_matrix(mean_thread, NO_THREADS, MAX_DEPTH + 1);
    _zero_dbl_matrix(meansq_thread, NO_THREADS, MAX_DEPTH + 1);
    _zero_dbl_matrix(P0Sum_thread, NO_THREADS, MAX_DEPTH + 1);

	return 0;
}

static int _process1_computeNullModel(double** simMatrix, double*** P0, 
	                                  double** mean_thread, double** meansq_thread,
                                      double** P0Sum_thread, igraph_integer_t* degs,
                                      igraph_integer_t N, igraph_integer_t n){
    int tid = omp_get_thread_num();
    double* me_t = mean_thread[tid];
    double* mes_t = meansq_thread[tid];
    double* P0Sum_t = P0Sum_thread[tid];
    igraph_integer_t deg1_t = degs[n];
    double* simRowBase = simMatrix[n] - (n + 1);

    for (igraph_integer_t m = n + 1; m < N; m++) {
        igraph_integer_t deg2_t = degs[m];
        double ddummy_t = simRowBase[m];
        for (int l = 0; l < MAX_DEPTH + 1; l++) {
            double ddummy2_t = (P0[l + 1][deg1_t][deg2_t] + P0[l + 1][deg2_t][deg1_t]);
            double ddummy3_t = ddummy_t * ddummy2_t;
            me_t[l] += ddummy3_t;
            mes_t[l] += ddummy_t * ddummy3_t;
            P0Sum_t[l] += ddummy2_t;
        }
    }

    return 0;
}


static inline int _process3_computeNullModel(double** mean_thread, double** meansq_thread, double** P0Sum_thread,
	                                         double* meanSim, double* stdSim, double* P0Sum) {

    for (int l = 0; l < MAX_DEPTH + 1; l++) {
        for (int t = 0; t < NO_THREADS; t++) {
            meanSim[l] += mean_thread[t][l];
            P0Sum[l] += P0Sum_thread[t][l];
            if (stdSim != NULL) {
                stdSim[l] += meansq_thread[t][l];
            }
        }
        if (P0Sum[l] > 0) {
            meanSim[l] /= P0Sum[l];
            if (stdSim != NULL) {
                stdSim[l] /= P0Sum[l];
				stdSim[l] -= meanSim[l] * meanSim[l];
				stdSim[l] = sqrt(stdSim[l] > 0.0 ? stdSim[l] : 0.0);
            }
        }
        else {
            meanSim[l] = 0.0;
            if (stdSim != NULL) {
                stdSim[l] = 0.0;
            }
        }
    }

    return 0;
}

static inline int _process3_computeNullModelTrueDegree(double** mean_thread, double** meansq_thread, double** P0Sum_thread,
    double* meanSim, double* stdSim, double* P0Sum) {

    for (int l = 0; l < MAX_DEPTH + 1; l++) {
        for (int t = 0; t < NO_THREADS; t++) {
            meanSim[l] += mean_thread[t][l];
            P0Sum[l] += P0Sum_thread[t][l];
            if (stdSim != NULL) {
                stdSim[l] += meansq_thread[t][l];
            }
        }
        if (P0Sum[l] > 0) {
            meanSim[l] /= P0Sum[l];
            if (stdSim != NULL) {
                stdSim[l] /= P0Sum[l];
                stdSim[l] -= meanSim[l];
                stdSim[l] = sqrt(stdSim[l] > 0.0 ? stdSim[l] : 0.0);
            }
        }
        else {
            meanSim[l] = 0.0;
            if (stdSim != NULL) {
                stdSim[l] = 0.0;
            }
        }
    }

    return 0;
}

int computeNullModel(igraph_t* graph, double** simMatrix, igraph_integer_t N, double meanSim[], double stdSim[] ) {
    igraph_integer_t n;
    igraph_vector_int_t degrees;
    igraph_integer_t deg, maxdegree = 0;
    int k, kp, l = 0;
    double kmean, x, G1 = 0.0;
    double* Pk = nullptr;
    double** qbar = nullptr;
    double*** rho = nullptr;
    double*** P0 = nullptr;
    double P0Sum[MAX_DEPTH + 1] = { 0.0 };
    double** mean_thread = nullptr;
    double** meansq_thread = nullptr;
    double** P0Sum_thread = nullptr;
    igraph_integer_t* degs = nullptr;
    int level = 0;

    printf("Computing null model - Degree sequence\n");
    /**********************
        MEMORY ALLOCATION
     **********************/
    if( _allocate_computeNullModel_resources(graph, degrees, Pk, qbar, rho, P0, mean_thread, meansq_thread, P0Sum_thread, maxdegree, level) != 0) {
        return -1;
	}
    degs = VECTOR(degrees);

    /****************
        LET'S WORK
     ****************/
    kmean = 0.0;
    for (n = 0; n < N; n++) {
        deg = degs[n];
        Pk[deg] += 1.0;
        kmean += (double)deg;
    }
    kmean /= (double)N;
    for (k = 0; k < maxdegree + 1; k++) {
        Pk[k] /= (double)N;
    }

    for (kp = 1; kp < maxdegree + 1; kp++) {
        qbar[0][kp] = ((double)kp) / (kmean * (double)N);
        for (l = 1; l < MAX_DEPTH + 1; l++) {
            x = 1 - qbar[l - 1][kp];
            G1 = 0.0;
            /*for (k = 0; k < maxdegree + 1; k++) {
                G1 += ((double)k) * Pk[k] * pow(x, (double)(k - 1));
            }*/
			double xpow = 1.0;
            double xpowkpm1 = 1.0;
            for(k = 1; k < maxdegree + 1; k++) {
                G1 += ((double)k) * Pk[k] * xpow;
                if (k == kp) {
					xpowkpm1 = xpow;
                }
				xpow *= x;
			}
            G1 /= kmean;
//            qbar[l][kp] = 1 - G1 + kp / (kmean * (double)N) * pow(x, (double)(kp - 1));
            qbar[l][kp] = 1 - G1 + kp / (kmean * (double)N) * xpowkpm1;
        }
    }

    for (k = 0; k < maxdegree + 1; k++) {
        for (kp = 0; kp < maxdegree + 1; kp++) {
            if (k == kp) {
                if (Pk[k] > 0) {
                    rho[0][k][kp] = 1.0 / (Pk[k] * (double)N);
                }
                else {
                    rho[0][k][kp] = 0.0;
                }
            }
            else {
                rho[0][k][kp] = 0.0;
            }
            P0[0][k][kp] = rho[0][k][kp];
            for (l = 1; l < MAX_DEPTH + 1; l++) {
                rho[l][k][kp] = 1.0 - (1.0 - rho[0][k][kp]) * pow(1 - qbar[l - 1][kp], (double)k);
                P0[l][k][kp] = rho[l][k][kp] - rho[l - 1][k][kp];
            }
            P0[MAX_DEPTH + 1][k][kp] = 1.0 - rho[MAX_DEPTH][k][kp];
        }
    }


#pragma omp parallel for num_threads(NO_THREADS)
    for (n = 0; n < N; n++) {
        _process1_computeNullModel(simMatrix, P0, mean_thread, meansq_thread, P0Sum_thread, degs, N, n);
    }
    _process3_computeNullModel(mean_thread, meansq_thread, P0Sum_thread, meanSim, stdSim, P0Sum);

    /****************
        FREE MEMORY
     ****************/
    _free_computeNullModel_resources(&degrees, Pk, qbar, rho, P0, mean_thread, meansq_thread, P0Sum_thread, maxdegree, level);
    printf("Null model computed\n");

    return 0;
}


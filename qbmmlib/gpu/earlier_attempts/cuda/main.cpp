#include <cassert>
#include "main.hpp"


/* print a usage message */
void usage(int argc, char **argv) {
    printf("usage: %s num_moments num_thread \n", argv[0]);
    return;
}

/* Set up inital moment inputs */
void init_input(float* moments, int size) {

    float one_moment[] = {1, 1, 1, 1.01, 1, 1.01};
    for (int i = 0; i< size * 6; i+= 6) {
        memcpy((void*)&moments[i], &one_moment, sizeof(float) * 6);
    }
}

int main(int argc, char **argv) {

    if (argc != 3) {
        usage(argc, argv);
        return 1;
    }
    int num_moments = atoi(argv[1]);
    int num_thread = atoi(argv[2]);

    float *input_moments = new float[6*num_moments];
    float *x_out_cuda = new float[4*num_moments];
    float *x_out_omp = new float[4*num_moments];
    float *x_out_naive = new float[4*num_moments];
    float *y_out_cuda = new float[4*num_moments];
    float *y_out_omp = new float[4*num_moments];
    float *y_out_naive = new float[4*num_moments];
    float *w_out_cuda = new float[4*num_moments];
    float *w_out_omp = new float[4*num_moments];
    float *w_out_naive = new float[4*num_moments];
    init_input(input_moments, num_moments);

    float cuda_time = qmom_cuda(input_moments, num_moments, x_out_cuda, y_out_cuda, w_out_cuda);
    float omp_time = qmom_openmp(input_moments, num_moments, num_thread, x_out_omp, y_out_omp, w_out_omp);
    float naive_time = qmom_naive(input_moments, num_moments, x_out_naive, y_out_naive, w_out_naive);

    // for (int i = 0; i < num_moments; i++) {
    //     // printf(" %f ", x_out_cuda[i]);
    //     // printf(" %f ", x_out_omp[i]);
    //     // printf(" %f ", y_out_cuda[i]);
    //     // printf(" %f ", y_out_omp[i]);
    //     // printf(" %f ", w_out_cuda[i]);
    //     // printf(" %f ", w_out_omp[i]);
    //     // printf(" \n");
    //     assert(x_out_cuda[i] == x_out_omp[i]);
    //     assert(y_out_cuda[i] == y_out_omp[i]);
    //     assert(w_out_cuda[i] == w_out_omp[i]);

    // }
    for (int i = 0; i < num_moments; i++) {
        if (y_out_cuda[i] != y_out_omp[i]) {
            fprintf(stderr, " \n");
            fprintf(stderr, " x_cuda: %f ", x_out_cuda[i]);
            fprintf(stderr, " x_omp: %f \n", x_out_omp[i]);
            fprintf(stderr, " y_cuda: %f ", y_out_cuda[i]);
            fprintf(stderr, " y_omp: %f \n", y_out_omp[i]);
            fprintf(stderr, " w_cuda: %f ", w_out_cuda[i]);
            fprintf(stderr, " w_omp: %f \n", w_out_omp[i]);

            fprintf(stderr, "Error at index %d: ", i);
            fprintf(stderr, " %f ", y_out_cuda[i]);
            fprintf(stderr, " %f \n", y_out_omp[i]);    
            throw;
            // assert(false);
        }
    }
    printf("Input size: %d \n", num_moments);
    printf("[CUDA]    Took %e s per input \n", cuda_time/num_moments*1e-3);
    printf("[OPEN_MP] Took %e s per input \n", omp_time/num_moments*1e-3);
    printf("[NAIVE]   Took %e s per input \n", naive_time/num_moments*1e-3);

    delete[] input_moments;
    delete[] x_out_cuda;
    delete[] x_out_omp;
    delete[] x_out_naive;
    delete[] y_out_cuda;
    delete[] y_out_omp;
    delete[] y_out_naive;
    delete[] w_out_cuda;
    delete[] w_out_omp;
    delete[] w_out_naive;

    return 0;
}
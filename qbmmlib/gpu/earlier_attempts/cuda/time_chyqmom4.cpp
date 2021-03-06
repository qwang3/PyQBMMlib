#include <cstring>
#include <cstdlib>
#include <cassert>
#include <string>
#include <fstream>

#include <omp.h>

#include "hyqmom.hpp"
#include "main.hpp"

/* print out a usage message */
void usage(int argc, char **argv) {
    fprintf(stderr, "usage: %s filename max_input stride omp_nthreads\n", argv[0]);
}

/* Set up inital moment inputs for chyqmom4*/
void init_input_6(float moments[], int size) {
    // data obtained from running stats.raw_gaussian_moments_bivar
    float one_moment[6] = {1.0, 1.0, 1.0, 1.01, 1.0, 1.01};
    for (int i = 0; i< size; i++) {
        for (int j = 0; j < 6; j++) {
            moments[i + j*size] = one_moment[j];
        }
    }
}

int main(int argc, char **argv) {

    if (argc != 5) {
        usage(argc, argv);
        return 1;
    }

    std::string filename = argv[1];
    int N_max = atoi(argv[2]);
    float stride = std::stof(argv[3]);
    int omp_n_threads = atoi(argv[4]);

    std::ofstream result_file;
    char line[100];
    memset(line, 0, sizeof(char) * 100);
    result_file.open(filename);
    result_file << "Input Size, cuda_1 (ms), cuda_2 (ms), cuda_3 (ms)\n";
    
    for (float x_moments = 1; x_moments < N_max; x_moments*= stride) {

        int num_moments = (int) ceil(x_moments);
        printf("Running %d inputs \n", num_moments);
        //input 
        float *input_moments = new float[6*num_moments];
        float *x_out_cuda = new float[4*num_moments];
        float *y_out_cuda = new float[4*num_moments];
        float *w_out_cuda = new float[4*num_moments];
        init_input_6(input_moments, num_moments);

        // output results in row major format
        float cuda_time1 = chyqmom4(input_moments, num_moments, w_out_cuda, x_out_cuda, y_out_cuda, 1);
        float cuda_time2 = chyqmom4(input_moments, num_moments, w_out_cuda, x_out_cuda, y_out_cuda, 1);
        float cuda_time3 = chyqmom4(input_moments, num_moments, w_out_cuda, x_out_cuda, y_out_cuda, 1);

        sprintf(line, "%d, %f, %f, %f\n", num_moments, cuda_time1, cuda_time2, cuda_time3);
        result_file << line;
        memset(line, 0, sizeof(char) * 100);
        
        delete[] input_moments;
        delete[] x_out_cuda;
        delete[] y_out_cuda;
        delete[] w_out_cuda;
    }
    result_file.close();
    return 0;
}
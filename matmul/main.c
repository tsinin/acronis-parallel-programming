#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "src/matmul.h"


int main(int argc, char** argv) {
    srand(0);

    uint32_t n = atoi(argv[2]);
    uint32_t m = atoi(argv[2]);
    uint32_t k = atoi(argv[2]);
    uint8_t thread_num = atoi(argv[3]);
    double* A = (double*)valloc(n * m * sizeof(double));
    double* B = (double*)valloc(m * k * sizeof(double));
    double* C1 = (double*)valloc(n * k * sizeof(double));
    double* C2 = (double*)valloc(n * k * sizeof(double));
    double* C3 = (double*)valloc(n * k * sizeof(double));
    for (uint32_t i = 0; i < n * m; ++i) {
        A[i] = (double)rand() / RAND_MAX * 10.0 - 5;
    }
    for (uint32_t i = 0; i < m * k; ++i) {
        B[i] = (double)rand() / RAND_MAX * 10.0 - 5;
    }
    struct timespec begin, end;
    double simple, b_restructured, multithreaded;


    if (strcmp(argv[1], "simple") == 0) {

        measure_time(matmul_simple_single(n, m, k, A, B, C1), simple)
        printf("%lf\n", simple);

    } else if (strcmp(argv[1], "optimized") == 0) {

        measure_time(matmul_B_restructured_single(n, m, k, A, B, C2), b_restructured)
        printf("%lf\n", b_restructured);

    } else if (strcmp(argv[1], "multi-threaded") == 0) {

        measure_time(matmul_multithread_blocks(n, m, k, A, B, C3, thread_num), multithreaded)
        printf("%lf\n", multithreaded);

    } else if (strcmp(argv[1], "all") == 0) {

        measure_time(matmul_simple_single(n, m, k, A, B, C1), simple)
        measure_time(matmul_B_restructured_single(n, m, k, A, B, C2), b_restructured)
        measure_time(matmul_multithread_blocks(n, m, k, A, B, C3, thread_num), multithreaded)

        if (check_equality(n * k, C1, C2) && check_equality(n * k, C1, C3)) {
            printf("Time with simple algo: %f\n", simple);
            printf("Time with cache-optimized algo: %f\n", b_restructured);
            printf("Time with cache-optimized multi-threadeded blocking algo: %f\n", multithreaded);
            printf("Acceleration of multi-threaded over simple: %lf\n", simple / multithreaded);
        } else {
            printf("Error: Answers are not equal\n");
        }

    }

    free(A);
    free(B);
    free(C1);
    free(C2);
    free(C3);
    return 0;
}
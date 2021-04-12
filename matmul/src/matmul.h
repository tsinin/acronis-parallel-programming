#ifndef MATMUL_H
#define MATMUL_H

#include <stdint.h>
#include <time.h>

/*
 * define for time measuring
 * Before using it, you should define "struct timespec begin, end;"
 * first argument is a function call with arguments
 * second argument is a double in which time will be placed
*/
#define measure_time(function_call, time) \
        clock_gettime(CLOCK_MONOTONIC, &begin); \
        function_call; \
        clock_gettime(CLOCK_MONOTONIC, &end); \
        time = end.tv_sec - begin.tv_sec + (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

/*
 * INTERFACE FOR ALL METHODS FOR MATRIX MULTIPLICATION
 * input: matrices A, B with sizes (size1, size2) and (size2, size3), in row-major data layout
 *        matrix C is output matrix in row-major data layout 
 *        (should be initialized with size (size1, size3))
*/

/*
 * Simple single-threaded matrix multiplication without optimizations
*/
void matmul_simple_single(uint32_t size1, uint32_t size2, uint32_t size3,
                            double* A, double* B, double* C);

/*
 * Single-threaded matrix multiplication with transposing B
 * for decreasing cache misses
*/
void matmul_B_restructured_single(uint32_t size1, uint32_t size2, uint32_t size3,
                                double* A, double* B, double* C);

struct task;

struct thread_args;

void* matmul_thread(void* pargs);

/*
 * Multi-threaded matrix multiplication with blocking and transposing B
*/
void matmul_multithread_blocks(uint32_t size1, uint32_t size2, uint32_t size3,
                                double* A, double* B, double* C, uint8_t THREAD_NUM);

/*
 * Checks equality of 2 double arrays
*/
int check_equality(uint32_t size, double* A, double* B);

#endif // #define MATMUL_H
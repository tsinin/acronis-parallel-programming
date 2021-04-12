#include "matmul.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


void matmul_simple_single(uint32_t size1, uint32_t size2, uint32_t size3,
                            double* A, double* B, double* C) {
    for (uint32_t i = 0; i < size1; ++i) {
        for (uint32_t j = 0; j < size3; ++j) {
            double element = 0;
            C[i * size3 + j] = 0;
            for (uint32_t k = 0; k < size2; ++k) {
                element += A[i * size2 + k] * B[k * size3 + j];
            }
            C[i * size3 + j] = element;
        }
    }
}

void matmul_B_restructured_single(uint32_t size1, uint32_t size2, uint32_t size3,
                                double* A, double* B, double* C) {
    double* B_T = malloc(size2 * size3 * sizeof(double));
    for (uint32_t i = 0; i < size2; ++i) {
        for (uint32_t j = 0; j < size3; ++j) {
            B_T[j * size2 + i] = B[i * size3 + j];
        }
    }
    for (uint32_t i = 0; i < size1; ++i) {
        for (uint32_t j = 0; j < size3; ++j) {
            double element = 0;
            for (uint32_t k = 0; k < size2; ++k) {
                element += A[i * size2 + k] * B_T[j * size2 + k];
            }
            C[i * size3 + j] = element;
        }
    }
    free(B_T);
}

int check_equality(uint32_t size, double* A, double* B) {
    for (uint32_t i = 0; i < size; ++i) {
        if (abs(A[i] - B[i]) > 1e-5) {
            printf("i=%d\nC1=%lf, C2=%lf\n", i, A[i], B[i]);
            return 0;
        }
    }
    return 1;
}

struct task {
    uint32_t x_start;
    uint32_t y_start;
    uint32_t x_end;
    uint32_t y_end;
};

struct thread_args {
    uint32_t size2;
    uint32_t size3;
    double* A;
    double* B_T;
    double* C;
    uint32_t tasks_num;
    struct task* tasks;
};

void* matmul_thread(void* pargs) {
    struct thread_args* args = (struct thread_args*)pargs;
    double* A = args->A;
    double* B_T = args->B_T;
    double* C = args->C;
    uint32_t size2 = args->size2;
    uint32_t size3 = args->size3;

    for (uint32_t task_id = 0; task_id < args->tasks_num; ++task_id) {
        struct task task = args->tasks[task_id];

        for (uint32_t i = task.x_start; i < task.x_end; ++i) {
            for (uint32_t j = task.y_start; j < task.y_end; ++j) {
                C[i * size3 + j] = 0;
                for (uint32_t k = 0; k < size2; ++k) {
                    C[i * size3 + j] += A[i * size2 + k] * B_T[j * size2 + k];
                }
            }
        }
    }
}

void matmul_multithread_blocks(uint32_t size1, uint32_t size2, uint32_t size3,
                                double* A, double* B, double* C, uint8_t THREAD_NUM) {
    uint32_t block_size = 127*127 / size2;
    if (block_size == 0) {
        block_size = 1;
    }
    uint32_t grid_x = (size1 / block_size + ((size1 % block_size) != 0));
    uint32_t grid_y = (size3 / block_size + ((size3 % block_size) != 0));
    uint32_t block_num = grid_x * grid_y;
    printf("Block size: %dx%d\nBlocks grid: %dx%d\nBlocks number: %d\n", 
            block_size, block_size, grid_x, grid_y, block_num);
    struct task tasks[block_num];
    for (uint32_t i = 0; i < grid_x; ++i) {
        for (uint32_t j = 0; j < grid_y; ++j) {
            tasks[i * grid_y + j].x_start = i * block_size;
            tasks[i * grid_y + j].y_start = j * block_size;
            tasks[i * grid_y + j].x_end = (i + 1) * block_size;
            if (tasks[i * grid_y + j].x_end > size1) {
                tasks[i * grid_y + j].x_end = size1;
            }
            tasks[i * grid_y + j].y_end = (j + 1) * block_size;
            if (tasks[i * grid_y + j].y_end > size3) {
                tasks[i * grid_y + j].y_end = size3;
            }
        }
    }

    double* B_T = malloc(size2 * size3 * sizeof(double));
    for (uint32_t i = 0; i < size2; ++i) {
        for (uint32_t j = 0; j < size3; ++j) {
            B_T[j * size2 + i] = B[i * size3 + j];
        }
    }

    uint32_t extra = block_num % THREAD_NUM;
    uint32_t offset = 0;
    uint32_t blocks_per_thread = block_num / THREAD_NUM;
    pthread_t thread_ids[THREAD_NUM];
    struct thread_args* args = (struct thread_args*)malloc(THREAD_NUM * sizeof(struct thread_args));
    for (uint32_t i = 0; i < THREAD_NUM; ++i) {
        args[i].size2 = size2;
        args[i].size3 = size3;
        args[i].tasks_num = blocks_per_thread;
        args[i].A = A;
        args[i].B_T = B_T;
        args[i].C = C;

        if (extra > 0) {
            args[i].tasks_num++;
            extra--;
        }
        args[i].tasks = (struct task*)malloc(args->tasks_num * sizeof(struct task));

        for (uint32_t task_id = 0; task_id < args[i].tasks_num; task_id++) {
            args[i].tasks[task_id] = tasks[task_id + offset];
        }
        offset += args[i].tasks_num;
        pthread_create(&thread_ids[i], NULL, matmul_thread, (void*)&args[i]);
    }

    for (uint32_t i = 0; i < THREAD_NUM; ++i) {
        pthread_join(thread_ids[i], NULL);
    }
    for (uint32_t i = 0; i < THREAD_NUM; ++i) {
        free(args[i].tasks);
    }
    free(args);
    free(B_T);
}
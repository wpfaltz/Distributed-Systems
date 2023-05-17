#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

#define N 1000000000

volatile atomic_flag lock = ATOMIC_FLAG_INIT;
long long int sum = 0;
int num_threads;

char *vector;

void acquire() {
    while (atomic_flag_test_and_set(&lock));// busy wait
}

void release() {
    atomic_flag_clear(&lock);
}

void* calculate_sum(void* arg) {
    int tid = *(int*)arg;
    int num_threads = *(int*)(arg + sizeof(int));
    int start = tid * (N/num_threads);
    int end = (tid == num_threads - 1) ? N : (tid + 1) * (N/num_threads);
    int local_sum = 0;
    for (int i = start; i < end; i++) {
        local_sum += (int)vector[i];
    }
    acquire();
    sum += local_sum;
    release();
    return NULL;
}

int main(int argc, char** argv) {
    vector = malloc(N * sizeof(char));
    srand(time(NULL)); //inicializa o gerador de números aleatórios com a hora
    for (int i = 0; i < N; i++) {
        vector[i] = (rand() % 201) - 100; // preenche o vetor com números aleatórios no intervalo [-100, 100]
    }
    int num_threads_list[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
    int num_cases = sizeof(num_threads_list) / sizeof(num_threads_list[0]);

    for (int case_idx = 0; case_idx < num_cases; case_idx++) {
        int num_threads = num_threads_list[case_idx];
        pthread_t threads[num_threads];
        int thread_ids[num_threads][2];
        sum = 0;
        for (int i = 0; i < num_threads; i++) {
            thread_ids[i][0] = i;
            thread_ids[i][1] = num_threads;
            pthread_create(&threads[i], NULL, calculate_sum, &thread_ids[i]);
        }

        struct timespec start, end;
        clock_gettime(CLOCK_REALTIME, &start);
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
        clock_gettime(CLOCK_REALTIME, &end);
        double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Número de threads: %d\n", num_threads);
        printf("Soma: %lld\n", sum);
        printf("Tempo de execução: %lf segundos\n", elapsed_time);
        printf("---------------------------------------\n");
    }
    free(vector); // libera a memória alocada para o vetor
    return 0;
}

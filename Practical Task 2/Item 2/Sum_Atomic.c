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

int *vector;

void acquire() {
    while (atomic_flag_test_and_set(&lock)); // busy wait
}

void release() {
    atomic_flag_clear(&lock);
}

void* calculate_sum(void* arg) {
    int tid = *(int*)arg;
    int start = tid * (N/num_threads);
    int end = (tid+1) * (N/num_threads);
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
    if (argc < 2) {
        printf("Uso: %s numero_threads\n", argv[0]);
        return 1;
    }
    num_threads = atoi(argv[1]);
    vector = malloc(N * sizeof(int));
    srand(time(NULL)); //inicializa o gerador de números aleatórios com a hora
    for (int i = 0; i < N; i++) {
        vector[i] = (rand() % 201) - 100; // preenche o vetor com números aleatórios no intervalo [-100, 100]
    }
    pthread_t threads[num_threads];
    int thread_ids[num_threads];
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, calculate_sum, &thread_ids[i]);
    }
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("sum: %lld\n", sum);
    printf("elapsed time: %lf seconds\n", elapsed_time);
    free(vector); // libera a memória alocada para o vetor
    return 0;
}

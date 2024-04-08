#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N_TRUCKS 30
#define N_MACHINE 30
#define N_FACTORY 15

#define MAX_TIME_FACTORY 5
#define MAX_TIME_MACHINE 10
#define MAX_TIME_TRUCK 10

#define WAIT_TIME 5
#define SLOWDOWN 250000

typedef struct Deposit_t {
  size_t machines_itens, truck_itens;
} Deposit;

Deposit deposit = {0};
pthread_mutex_t deposit_access;
pthread_barrier_t start_barrier;

void *factory(void *p) {
  pthread_barrier_wait(&start_barrier);

  long id = (long)p;

  while (1) {
    pthread_mutex_lock(&deposit_access);
    if (rand() % 2) {
      deposit.machines_itens++;
      printf("[FACTORY %ld] produziu item de maquina\n", id);
    } else {
      deposit.truck_itens++;
      printf("[FACTORY %ld] produziu item de caminhao\n", id);
    }
    usleep(SLOWDOWN);
    pthread_mutex_unlock(&deposit_access);
    sleep(rand() % MAX_TIME_FACTORY + 1);
  }
}

void *machine(void *p) {
  pthread_barrier_wait(&start_barrier);

  long id = (long)p;

  while (1) {
    pthread_mutex_lock(&deposit_access);

    if (deposit.machines_itens != 0) {

      deposit.machines_itens--;
      printf("[MACHINE %ld] consumiu item\n", id);
      usleep(SLOWDOWN);

      pthread_mutex_unlock(&deposit_access);
      sleep(rand() % MAX_TIME_MACHINE + 1);
    } else {
      pthread_mutex_unlock(&deposit_access);
      sleep(WAIT_TIME);
    }
  }
}

void *truck(void *p) {
  pthread_barrier_wait(&start_barrier);

  long id = (long)p;

  while (1) {
    pthread_mutex_lock(&deposit_access);

    if (deposit.truck_itens != 0) {

      deposit.truck_itens--;
      printf("[TRUCK %ld] consumiu item\n", id);

      usleep(SLOWDOWN);

      pthread_mutex_unlock(&deposit_access);

      sleep(rand() % MAX_TIME_TRUCK + 1);
    } else {
      pthread_mutex_unlock(&deposit_access);
      sleep(WAIT_TIME);
    }
  }
}

int main() {

  srand(time(NULL));

  printf("Starting factory\n");

  pthread_t factory_thread[N_FACTORY], machine_threads[N_MACHINE],
      truck_threads[N_TRUCKS];

  pthread_mutex_init(&deposit_access, NULL);
  pthread_barrier_init(&start_barrier, NULL,
                       N_FACTORY + N_MACHINE + N_TRUCKS + 1);

  for (long i = 0; i < N_FACTORY; i++)
    pthread_create(&factory_thread[i], NULL, factory, (void *)i);

  for (long i = 0; i < N_MACHINE; i++)
    pthread_create(&machine_threads[i], NULL, machine, (void *)i);

  for (long i = 0; i < N_TRUCKS; i++)
    pthread_create(&machine_threads[i], NULL, truck, (void *)i);

  pthread_barrier_wait(&start_barrier);

  while (1) {
    sleep(2);
    printf("[DEPOSIT] |machine_itens: %zu | truck_itens: %zu|\n",
           deposit.machines_itens, deposit.truck_itens);
  }

  for (int i = 0; i < N_FACTORY; i++)
    pthread_join(factory_thread[i], NULL);
  for (int i = 0; i < N_MACHINE; i++)
    pthread_join(machine_threads[i], NULL);
  for (int i = 0; i < N_TRUCKS; i++)
    pthread_join(truck_threads[i], NULL);

  return 0;
}
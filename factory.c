#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N_TRUCKS 30
#define N_MACHINE 30
#define N_FACTORY 15

#define MAX_TIME_FACTORY 5  // tempo maximo de produção
#define MAX_TIME_MACHINE 10 // tempo maximo de consumo das maquinas
#define MAX_TIME_TRUCK 10   // tempo maximo de consumo dos caminhões

#define WAIT_TIME 5     // tempo de espera caso o deposito estiver vazio
#define SLOWDOWN 250000 // desaceleração prosital de execução

typedef struct Deposit_t {
  size_t machines_itens, truck_itens;
} Deposit;

Deposit deposit = {0}; // inicialização do deposito

pthread_mutex_t deposit_access; // mutex de acesso à variavel de acesso
                                // compartilhada (deposito)

pthread_barrier_t start_barrier; // barreira de inicialização

void *factory(void *p) {
  pthread_barrier_wait(
      &start_barrier); // espera até que todos as estruturas estejam prontas

  long id = (long)p;

  while (1) {
    sleep(rand() % MAX_TIME_FACTORY + 1); // simula tempo de produção
    pthread_mutex_lock(
        &deposit_access); // garante o acesso à variavel compartilhada

    if (rand() % 2) {
      deposit.machines_itens++; // produz item de maquina
      printf("[FACTORY %ld] produziu item de maquina\n", id);
    } else {
      deposit.truck_itens++; // produz item de caminhão
      printf("[FACTORY %ld] produziu item de caminhao\n", id);
    }

    usleep(SLOWDOWN); // desacelera propositalmente o programa
    pthread_mutex_unlock(
        &deposit_access); // libera o acesso à variavel compartilhada
  }
}

void *machine(void *p) {
  pthread_barrier_wait(
      &start_barrier); // espera até que todos as estruturas estejam prontas

  long id = (long)p;

  while (1) {
    pthread_mutex_lock(
        &deposit_access); // garante o acesso à variavel compartilhada

    if (deposit.machines_itens != 0) { // verifica se o existem itens

      deposit.machines_itens--; // se existir, consome item
      printf("[MACHINE %ld] consumiu item\n", id);
      usleep(SLOWDOWN); // desacelera propositalmente o programa

      pthread_mutex_unlock(
          &deposit_access); // libera o acesso à variavel compartilhada
      sleep(rand() % MAX_TIME_MACHINE + 1); // simula tempo de consumo
    } else {
      pthread_mutex_unlock(&deposit_access); // se não existir, libera acesso à
                                             // variavel compartilhada
      sleep(WAIT_TIME); // espera até proxima checagem
    }
  }
}

void *truck(void *p) {
  pthread_barrier_wait(
      &start_barrier); // espera até que todos as estruturas estejam prontas

  long id = (long)p;

  while (1) {
    pthread_mutex_lock(
        &deposit_access); // garante o acesso à variavel compartilhada

    if (deposit.truck_itens != 0) { // verifica se o existem itens

      deposit.truck_itens--; // se existir, consome item
      printf("[TRUCK %ld] consumiu item\n", id);

      usleep(SLOWDOWN); // desacelera propositalmente o programa

      pthread_mutex_unlock(
          &deposit_access); // libera o acesso à variavel compartilhada

      sleep(rand() % MAX_TIME_TRUCK + 1); // simula tempo de consumo
    } else {
      pthread_mutex_unlock(&deposit_access); // se não existir, libera acesso à
                                             // variavel compartilhada
      sleep(WAIT_TIME); // espera até proxima checagem
    }
  }
}

int main() {

  srand(time(NULL));

  printf("Starting factory\n");

  pthread_t factory_thread[N_FACTORY], machine_threads[N_MACHINE],
      truck_threads[N_TRUCKS];

  pthread_mutex_init(&deposit_access,
                     NULL); // inicializa mutex do acesso ao deposito
  pthread_barrier_init(&start_barrier, NULL,
                       N_FACTORY + N_MACHINE + N_TRUCKS +
                           1); // inicializa barreira de inicialização

  for (long i = 0; i < N_FACTORY; i++) // cria as threads das fabricas
    pthread_create(&factory_thread[i], NULL, factory, (void *)i);

  for (long i = 0; i < N_MACHINE; i++) // cria as threads das maquinas
    pthread_create(&machine_threads[i], NULL, machine, (void *)i);

  for (long i = 0; i < N_TRUCKS; i++) // cria as threads dos caminhões
    pthread_create(&machine_threads[i], NULL, truck, (void *)i);

  pthread_barrier_wait(
      &start_barrier); // espera até que todos as estruturas estejam prontas

  while (1) { // printa o estado atual do deposito
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
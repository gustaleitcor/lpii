#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void *atomic_add(void *arg);
void *mutual_exclusion_add(void *arg);
void *wait_busy_lock(void *arg);

struct atomic_args {
  int *arg1;

  int increment;
  int qtd_increment;
};

struct mutual_exclusion_args {
  pthread_mutex_t *mutex;
  int *arg1;

  int increment;
  int qtd_increment;
};

struct wait_busy_lock_args {
  int *lock;
  int *arg1;

  int increment;
  int qtd_increment;
};

int main() {
  int var_global = 0, increment, qtd_increment;
  size_t n_threads;
  pthread_t *threads;
  clock_t start, end;
  double seconds;

  printf("Digite a quantidade de threads a serem criadas: ");
  scanf("%zu", &n_threads);
  printf("Digite o incremento de cada thread: ");
  scanf("%d", &increment);
  printf("Digite a quantidade de incrementos de cada thread: ");
  scanf("%d", &qtd_increment);

  threads = (pthread_t *)malloc(sizeof(pthread_t) * n_threads);

  // Função atômica

  printf("\n--- Iniciando a funcao atomica ---\n");

  // Function params
  struct atomic_args atomic_input = {.arg1 = &var_global,
                                     .increment = increment,
                                     .qtd_increment = qtd_increment};

  start = clock();

  for (int i = 0; i < n_threads; i++)
    pthread_create(&threads[i], NULL, atomic_add, (void *)&atomic_input);

  for (int i = 0; i < n_threads; i++)
    pthread_join(threads[i], NULL);

  end = clock();

  seconds = (double)(end - start) / CLOCKS_PER_SEC;

  printf("Duracao: %lf (s) - Soma: %d\n", seconds, var_global);

  printf("--- Terminada a funcao atomica ---\n");

  // Exclusão mútua

  var_global = 0;

  pthread_mutex_t mutex;

  pthread_mutex_init(&mutex, NULL);

  // Function params
  struct mutual_exclusion_args mutual_exclusion_input = {.mutex = &mutex,
                                                         .arg1 = &var_global,
                                                         .increment = increment,
                                                         .qtd_increment =
                                                             qtd_increment};

  printf("\n--- Iniciando a exclusao mutua ---\n");

  start = clock();

  for (int i = 0; i < n_threads; i++)
    pthread_create(&threads[i], NULL, mutual_exclusion_add,
                   (void *)&mutual_exclusion_input);

  for (int i = 0; i < n_threads; i++)
    pthread_join(threads[i], NULL);

  end = clock();

  seconds = (double)(end - start) / CLOCKS_PER_SEC;

  printf("Duracao: %lf (s) - Soma: %d\n", seconds, var_global);

  printf("--- Terminando a exclusao mutua ---\n");

  pthread_mutex_destroy(&mutex);

  // Trava com espera ocupada

  int lock = 0;
  var_global = 0;

  // Function params
  struct wait_busy_lock_args wait_busy_lock_input = {.lock = &lock,
                                                     .arg1 = &var_global,
                                                     .increment = increment,
                                                     .qtd_increment =
                                                         qtd_increment};

  printf("\n--- Iniciando a trava com espera ocupada ---\n");

  start = clock();

  for (int i = 0; i < n_threads; i++)
    pthread_create(&threads[i], NULL, wait_busy_lock,
                   (void *)&wait_busy_lock_input);

  for (int i = 0; i < n_threads; i++)
    pthread_join(threads[i], NULL);

  end = clock();

  seconds = (double)(end - start) / CLOCKS_PER_SEC;

  printf("Duracao: %lf (s) - Soma: %d\n", seconds, var_global);

  printf("--- Terminando a trava com espera ocupada ---\n\n");

  free(threads);

  return 0;
}

void *atomic_add(void *arg) {
  struct atomic_args *input = (struct atomic_args *)arg;

  for (int i = 0; i < input->qtd_increment; i++) {
    __atomic_fetch_add(input->arg1, input->increment, __ATOMIC_SEQ_CST);
  }

  return NULL;
}

void *mutual_exclusion_add(void *arg) {
  struct mutual_exclusion_args *input = (struct mutual_exclusion_args *)arg;

  for (int i = 0; i < input->qtd_increment; i++) {
    pthread_mutex_lock(input->mutex);
    (*input->arg1) += input->increment;
    pthread_mutex_unlock(input->mutex);
  }

  return NULL;
};

void *wait_busy_lock(void *arg) {
  struct wait_busy_lock_args *input = (struct wait_busy_lock_args *)arg;
  for (int i = 0; i < input->qtd_increment; i++) {
    while (__sync_lock_test_and_set(input->lock, 1)) {
    }

    (*input->arg1) += input->increment;
    (*input->lock) = 0;
  }
  return NULL;
}

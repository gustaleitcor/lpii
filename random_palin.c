#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_CHAR 7

struct param {
  char *word;
  int i;
  pthread_barrier_t *print_barrier;
  pthread_barrier_t *render_barrier;
};

void *randChar(void *p) {

  struct param *param = (struct param *)p;

  param->word[param->i] = (rand() % ('Z' - 'A' + 1)) + 'A';

  free(p);

  pthread_barrier_wait(param->render_barrier);

  pthread_barrier_wait(param->print_barrier);

  return NULL;
}

int is_palin(char *word) {
  for (int i = 0; i < N_CHAR / 2; i++) {
    if (word[i] != word[N_CHAR - i - 1])
      return 0;
  }

  return 1;
}

int main() {
  pthread_barrier_t print_barrier;
  pthread_barrier_t render_barrier;
  char word[N_CHAR];
  pthread_t threads[N_CHAR];

  srand(time(NULL));

  pthread_barrier_init(&print_barrier, NULL, N_CHAR + 1);
  pthread_barrier_init(&render_barrier, NULL, N_CHAR + 1);

  for (int j = 0;; j++) {
    for (int i = 0; i < N_CHAR; i++) {
      struct param *p = malloc(sizeof(struct param));
      p->i = i;
      p->print_barrier = &print_barrier;
      p->render_barrier = &render_barrier;
      p->word = word;
      pthread_create(&threads[i], NULL, randChar, (void *)p);
    }

    pthread_barrier_wait(&render_barrier);
    for (int i = 0; i < N_CHAR; i++) {
      printf("%c", word[i]);
    }

    if (is_palin(word)) {
      printf(" eh palindromo");
      printf("\n");
      return 0;
    } else {
      printf(" nao eh palindromo");
    }
    printf("\n");
    pthread_barrier_wait(&print_barrier);

    for (int i = 0; i < N_CHAR; i++) {
      pthread_join(threads[i], NULL);
    }
  }
  return 0;
}
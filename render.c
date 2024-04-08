#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// int frame[WIDTH][HEIGHT];

pthread_barrier_t show_barrier, render_barrier;

void *render_frame(void *p) {
  while (1) {
    //(...) Renderização
    printf("Renderizando...\n");
    sleep(2);
    pthread_barrier_wait(&render_barrier);
    pthread_barrier_wait(&show_barrier);
  }
}

void *show_frame(void *p) {
  while (1) {
    pthread_barrier_wait(&render_barrier);
    //(...) Mostragem
    printf("Mostragem...\n");
    pthread_barrier_wait(&show_barrier);
  }
}

int main() {

  pthread_t t1, t2;

  pthread_barrier_init(&show_barrier, NULL, 2);
  pthread_barrier_init(&render_barrier, NULL, 2);

  pthread_create(&t1, NULL, render_frame, NULL);
  pthread_create(&t1, NULL, show_frame, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
}

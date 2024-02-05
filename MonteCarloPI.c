//  EXERCÍCIO - PROGRAMAÇÃO CONCORRENTE
//  UFPB - 2023.1

//  1. Faça um programa em C/C++ para calcular o valor de pi usando o método de
//  Monte Carlo. O programa deve solicitar a quantidade de processos e criar os
//  processos descendentes - cada um dos quais deve gerar um número aleatório de
//  pontos e calcular o número de pontos que caem dentro do círculo unitário. O
//  processo principal deve então somar os resultados dos processos descendentes
//  para obter uma estimativa do valor de pi.

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main() {
  size_t n_process;
  unsigned long long x, y;
  const unsigned long long r_squared =
      ((unsigned long long)__INT_MAX__ * (unsigned long long)__INT_MAX__);
  long double inner_circle = 0;

  scanf("%zu", &n_process);

  pid_t pids[n_process];

  // int -> 4 Bytes
  // long / long long -> 8 bytes
  // long double -> 16 bytes

  for (unsigned int i = 0; i < n_process; i++) {
    pid_t pid = fork();
    if (pid != 0) {
      pids[i] = pid;
      printf("Process %u initialized\n", i);
      continue;
    }

    srand(getpid());

    for (unsigned int j = 0; j < 256; j++) {
      x = (unsigned long long)rand();
      y = (unsigned long long)rand();

      if (x * x + y * y <= r_squared) {
        inner_circle++;
      }
    }

    exit(inner_circle);
  }

  int stat;
  long double total = 0;

  for (unsigned int i = 0; i < n_process; i++) {
    wait(&stat);
    if (WIFEXITED(stat)) {
      stat = WEXITSTATUS(stat);
      printf("%d\n", stat);
      total += stat;
    } else {
      n_process--;
    }
  }

  printf("%Lf\n", 4 * (total / (n_process * 256)));

  return 0;
}

// int main() {
//   unsigned long long x, y;
//   const unsigned long long r_squared =
//       ((unsigned long long)__INT_MAX__ * (unsigned long long)__INT_MAX__);
//   long double total = 0, inner_circle = 0;

//   while (1) {
//     x = (unsigned long long)rand();
//     y = (unsigned long long)rand();

//     if (x * x + y * y <= r_squared) {
//       inner_circle++;
//     }
//     total++;

//     printf("%Lf\n", 4 * (inner_circle / total));
//   }
// }
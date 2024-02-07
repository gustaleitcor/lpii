#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_POINT_PER_PROCESS 1000000
#define N_PROCESS 1000

int monte_carlo(int ch[2]) {
  unsigned long long x, y;
  const unsigned long long r_squared =
      ((unsigned long long)__INT_MAX__ * (unsigned long long)__INT_MAX__);
  long double total = 0, inner_circle = 0;

  srand(time(NULL) + getpid());

  for (unsigned int i = 0; i < MAX_POINT_PER_PROCESS; i++) {
    x = (unsigned long long)rand();
    y = (unsigned long long)rand();

    if (x * x + y * y <= r_squared) {
      inner_circle++;
    }
  }

  write(ch[1], &inner_circle, sizeof(inner_circle));

  exit(EXIT_SUCCESS);
}

int main() {

  pid_t pid;
  pid_t pids[N_PROCESS];
  int ch[N_PROCESS][2];

  for (unsigned int i = 0; i < N_PROCESS; i++) {
    if (pipe(ch[i]) == -1) {
      return -1;
    }

    if ((pid = fork()) == 0) {
      return monte_carlo(ch[i]);
    } else {
      pids[i] = pid;
    }
  }

  long double total = 0;

  for (unsigned int i = 0; i < N_PROCESS; i++) {
    waitpid(pids[i], NULL, 0);
    long double response;
    read(ch[i][0], &response, sizeof(response));
    total += response;
  }

  printf("%Lf\n", 4 * (total / (MAX_POINT_PER_PROCESS * N_PROCESS)));

  return 0;
}
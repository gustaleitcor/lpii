#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define N_PROCESS 100
#define START 0
#define END 25
#define OFFSET 'A'
#define TARGET "ZZZZZZZ"

void add(char *s, size_t size, unsigned int op);
void fast_add(char *s, size_t size, unsigned int op);
unsigned long long int powu(int x, size_t y);

int is_eq(char *s1, char *s2, size_t size);

int divide_and_conquer(char *start, size_t size, unsigned int pace,
                       char *target, int ch[2]);

void prints(char *s, size_t size);

int main() {
  size_t n_char = strlen(TARGET);
  const unsigned long long int max = powu(26, n_char);
  pid_t pid;
  pid_t pids[N_PROCESS];
  int ch[2];

  if (pipe(ch) == -1)
    return -1;

  char s[sizeof(char) * n_char];
  char target[] = TARGET;

  for (unsigned int i = 0; i < n_char; i++) {
    s[i] = (char)START;
    target[i] -= OFFSET;
  }

  for (unsigned int i = 0; i < N_PROCESS - 1; i++) {
    if ((pid = fork()) == 0) {
      return divide_and_conquer(s, n_char, (max / N_PROCESS), target, ch);
    } else {
      pids[i] = pid;
    }
    fast_add(s, n_char, (max / N_PROCESS));
    prints(s, n_char);
  }

  if ((pid = fork()) == 0) {
    return divide_and_conquer(
        s, n_char, max - (N_PROCESS - 1) * (max / N_PROCESS), target, ch);
  } else {
    printf("pace: %llu\n", max - (N_PROCESS - 1) * (max / N_PROCESS));
    fast_add(s, n_char, max - (N_PROCESS - 1) * (max / N_PROCESS));

    prints(s, n_char);

    pids[N_PROCESS - 1] = pid;
  }

  printf("Waiting for response: \n");

  char buffer[sizeof(char) * n_char];

  read(ch[0], buffer, sizeof(char) * n_char);

  for (unsigned int i = 0; i < N_PROCESS; i++) {
    kill(pids[i], SIGINT);
  }

  prints(buffer, sizeof(char) * n_char);

  return 0;
}

void prints(char *s, size_t size) {
  for (unsigned int i = 0; i < size; i++) {
    printf("%c ", s[i] + OFFSET);
  }
  printf("\n");
}

unsigned long long int powu(int x, size_t y) {
  unsigned long long int response = 1;
  for (unsigned int i = 0; i < y; i++) {
    response *= x;
  }
  return response;
}

// !DEPRECATED DONT USE
// !THERES A BUG
void add(char *s, size_t size, unsigned int op) {
  s[0]++;
  for (unsigned int i = 0; i <= size - 1; i++) {
    if (s[i] > END) {
      s[i] = (char)START;
      s[i + 1]++;
    } else {
      break;
    }
  }
}

void fast_add(char *s, size_t size, unsigned int op) {
  unsigned int diff = END - START + 1;
  unsigned int remainder;

  for (unsigned int i = 0; (i < size) && (op != 0); i++) {
    remainder = (op + s[i]) % diff;
    s[i] = remainder;
    op = (op + s[i]) / diff;
  }
}

int divide_and_conquer(char *start, size_t size, unsigned int pace,
                       char *target, int ch[2]) {
  do {
    if (is_eq(start, target, size)) {
      printf("[%d] FOUND ", getpid());
      prints(start, size);
      write(ch[1], start, sizeof(char) * size);
    }

    fast_add(start, size, 1);
  } while (pace--);

  exit(EXIT_SUCCESS);
}

int is_eq(char *s1, char *s2, size_t size) {
  for (unsigned int i = 0; i < size; i++) {
    if (s1[i] != s2[i]) {
      return 0;
    }
  }
  return 1;
};

// char s[N_CHAR];
// for (unsigned int i = 0; i < N_CHAR; i++)
//   s[i] = (char)65;

// for (unsigned int i = 0; i < max; i++) {
//   for (unsigned int i = 0; i < N_CHAR; i++) {
//     printf("%c ", s[i]);
//   }
//   printf("\n");
//   add(s);
// }
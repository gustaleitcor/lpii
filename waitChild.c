// EXERCÍCIO - PROGRAMAÇÃO CONCORRENTE
// UFPB - 2023.1
//
// 2. Faça um programa em C/C++ que crie 10 processos que dormirão um tempo aleatório entre 5 e 20 segundos. Faça o processo principal dormir por 10 segundos e depois faça-o finalizar todos os processos descedentes que ainda estiverem ativos (use a função kill() para finalizar os processos descendentes).

#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <signal.h>

#define N_PROCESSES 10

int main()
{
    pid_t pids[N_PROCESSES], pid;

    for (unsigned int i = 0; i < N_PROCESSES; i++)
    {
        if ((pid = fork()) != 0)
        {
            pids[i] = pid;
            continue;
        }

        srand(getpid());

        unsigned int duration = (rand() % 15) + 6;

        printf("Process %d sleeping for %zu\n", getpid(), duration);

        sleep(duration);

        pids[i] = -1; // Does not work

        printf("Process %d awakes\n", getpid());

        return 0;
    }

    sleep(10);

    printf("killing remaining processes\n");

    for (unsigned int i = 0; i < N_PROCESSES; i++)
    {
        printf("killing process %d\n", pids[i]);

        kill(pids[i], SIGINT);
    }

    return 0;
}
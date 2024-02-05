#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    unsigned int n_process;
    unsigned long long x, y;
    const unsigned long long r_squared = ((unsigned long long)__INT_MAX__ * (unsigned long long)__INT_MAX__);
    long double total = 0, inner_circle = 0;

    scanf("%zu", &n_process);

    pid_t pids[n_process];

    // int -> 4 Bytes
    // long / long long -> 8 bytes
    // long double -> 16 bytes

    for (unsigned int i = 0; i < n_process; i++)
    {
        pid_t pid = fork();
        if (pid != 0)
        {
            pids[i] = pid;
            printf("Process %zu initialized\n", i);
            continue;
        }

        srand(getpid());

        for (unsigned int j = 0; j < 127; j++)
        {
            x = (unsigned long long)rand();
            y = (unsigned long long)rand();

            if (x * x + y * y <= r_squared)
            {
                inner_circle++;
            }
            total++;

            // printf("%llf\n", 4 * (inner_circle / total));
        }

        exit(inner_circle);
    }

    int stat;
    total = 0;

    for (unsigned int i = 0; i < n_process; i++)
    {
        if (WIFEXITED(stat))
        {
            waitpid(pids[i], &stat, 0);
            WEXITSTATUS(stat);
            total += stat;
        }
    }

    return 0;
}

// int main()
// {
//     unsigned int n_process;
//     unsigned long long x, y;
//     const unsigned long long r_squared = ((unsigned long long)__INT_MAX__ * (unsigned long long)__INT_MAX__);
//     long double total = 0, inner_circle = 0;

//     while (1)
//     {
//         x = (unsigned long long)rand();
//         y = (unsigned long long)rand();

//         if (x * x + y * y <= r_squared)
//         {
//             inner_circle++;
//         }
//         total++;

//         printf("%Lf\n", 4 * (inner_circle / total));
//     }
// }
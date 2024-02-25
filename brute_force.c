#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_LETRAS 26
#define ASCII_A 65

#define N_CHAR 4
#define START 'A'
#define END 'Z'

// Função encrypt
char *encrypt(const char *str, int tamanho);
// Função para pular para a proxima sequencia de caracteres Ex: AAAA -> BAAA
void add(char *s, size_t size, unsigned int op);
// Função que verifica se duas sequencias de caracteres são iguais
int is_eq(char *s1, char *s2, size_t size);

// Implementações das funções acima no rodapé do arquivo

int main() {

  pid_t file_pid, file_pids[10];

  // Inicia-se com a criação de um processo para cada arquivo
  for (unsigned int i = 0; i < 10; i++) {
    if ((file_pid = fork()) == 0) {

      // Abertura do arquivo para leitura
      char *file_format = "./senhas/%d.txt";
      char *file_path = (char *)malloc(sizeof(char) * strlen(file_format));
      sprintf(file_path, file_format, i);

      FILE *fp = fopen(file_path, "r");

      if (!fp) {
        return -1;
      }

      printf("[%d] Arquivo %s aberto\n", getpid(), file_path);

      char *target = NULL;
      size_t target_size;
      char *encrypted_str;
      char original_str[N_CHAR];
      unsigned int total_keys = 0;

      // Obtenção de cada sequência de caracteres alvo do arquivo
      while (getline(&target, &target_size, fp) != EOF) {
        // Contador para a quantidade de senhas
        total_keys++;

        // Setar a sequência de caracteres teste para AAAA
        for (int k = 0; k < N_CHAR; k++) {
          original_str[k] = (char)START;
        }

        // Criação de um processo para cada palavra
        if (fork() == 0) {
          // Encriptação da primeira palavra
          encrypted_str = encrypt(original_str, 4);
          // Enquanto a palavra encriptada teste for diferente da palavra alvo,
          // continua no while
          while (!is_eq(encrypted_str, target, 4)) {
            // Segue para a proxima sequencia. Ex: DCBA -> ECBA
            add(original_str, 4, 1);
            // Liberação da alocação da ultima palavra encriptada teste
            free(encrypted_str);
            // Criação de uma nova palavra encriptada teste
            encrypted_str = encrypt(original_str, 4);
          };

          // A solução é printada no terminal
          printf("%c%c%c%c -> %c%c%c%c\n", target[0], target[1], target[2],
                 target[3], original_str[0], original_str[1], original_str[2],
                 original_str[3]);

          // Por fim é liberado a alocação da ultima sequencia de caracteres
          // encriptada
          free(encrypted_str);
          free(target);
          return EXIT_SUCCESS;
        }
      }

      // Espera por todos os processos de cada palavra
      for (unsigned int j = 0; j < total_keys; j++) {
        wait(NULL);
      }

      // Liberação do restante das variaveis alocadas dinamicamente
      free(file_path);
      // Fechamento do arquivo
      fclose(fp);

      return EXIT_SUCCESS;
    } else {
      file_pids[i] = file_pid;
    }
  }

  // Espera por todos os processos de cada arquivo
  for (unsigned int i = 0; i < 10; i++) {
    wait(NULL);
  }

  return 0;
}

void add(char *s, size_t size, unsigned int op) {
  const unsigned int diff = END - START + 1;
  unsigned int remainder = 1;

  for (unsigned int i = 0; (i < size) && (op != 0); i++) {
    remainder = (op + s[i] - START) % diff;
    op = (op + s[i] - START) / diff;
    s[i] = remainder + START;
  }
}

int is_eq(char *s1, char *s2, size_t size) {
  for (unsigned int i = 0; i < size; i++) {
    if (s1[i] != s2[i]) {
      return 0;
    }
  }
  return 1;
};

char *encrypt(const char *str, int tamanho) {
  char *str_result = (char *)malloc(sizeof(char) * tamanho);
  for (int i = 0; i < tamanho; i++) {
    if (str[i] >= 'A' && str[i] <= 'Z') {
      int chave = str[i] - ASCII_A;
      str_result[i] = (str[i] - ASCII_A + chave) % NUM_LETRAS + ASCII_A;
    }
  }
  return str_result;
}

// Computação Paralela
// Nome: Thais Ferreira Cangucu - RA: 10403283
// Nome: Miguel Piñeiro Coratolo Simões - RA: 10427085
// Nome: Bruno Germanetti Ramalho - RA: 10426491
// Nome: Camila Nunes Carniel - RA: 10338558

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Le arquivo e processa linha por linha
void lerArquivo(const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "r");
    if (fp == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    long total_bytes = 0;
    int code_count = 0;

    while ((read = getline(&line, &len, fp)) != -1) {

        char *quote_ptr = strstr(line, "\" ");
        if (quote_ptr) {
            int status_code;
            long long bytes_sent;
            
            // Extrai status code e bytes
            if (sscanf(quote_ptr + 2, "%d %lld", &status_code, &bytes_sent) == 2) {
                total_bytes += bytes_sent;
                if(status_code == 404){
                    code_count++;
                }
            }
        }
    }
    printf("Total de Bytes = %ld Bytes\n", total_bytes);
    printf("Total de códigos 404 = %d\n", code_count);

    fclose(fp);

    if (line) {
        free(line);
    }
}


int main() {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    
    lerArquivo("access_log.txt");

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Tempo de execução: %.9f segundos\n", time_spent);

    return 0;
}

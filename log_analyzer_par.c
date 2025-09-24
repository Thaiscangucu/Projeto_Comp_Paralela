// Computação Paralela
// Nome: Thais Ferreira Cangucu - RA: 10403283
// Nome: Miguel Piñeiro Coratolo Simões - RA: 10427085
// Nome: Bruno Germanetti Ramalho - RA: 10426491
// Nome: Camila Nunes Carniel - RA: 10338558

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


typedef struct {
    char **lines;
    long long errors404;
    long long total_bytes;
    long inicio;
    long fim;
} Stats;


void *funcaoThreads(void *arg) {
    Stats *stats = (Stats *)arg;
    stats->errors404 = 0;
    stats->total_bytes = 0;

    for (long i = stats->inicio; i < stats->fim; i++) {
        char *line = stats->lines[i];

        // Processamento de uma única linha
        char *quote_ptr = strstr(line, "\" ");
        if (quote_ptr) {
            int status_code;
            long long bytes_sent;

            if (sscanf(quote_ptr + 2, "%d %lld", &status_code, &bytes_sent) == 2) {
                stats->total_bytes += bytes_sent;
                if (status_code == 404) {
                    stats->errors404++;
                }
            }
        }
    }
    return NULL;
}

// Transforma o arquivo em array
char **lerArquivoParaArray(const char *nomeArquivo, long *num_linhas) {
    FILE *fp = fopen(nomeArquivo, "r");
    if (fp == NULL) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    char **lines = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    *num_linhas = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        lines = realloc(lines, sizeof(char *) * (*num_linhas + 1));
        if (lines == NULL) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
        lines[*num_linhas] = strdup(line);
        (*num_linhas)++;
    }

    if (line) {
        free(line);
    }
    fclose(fp);

    return lines;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <número de threads> \n", argv[0]);
        return 1;
    }

    int NUM_THREADS = atoi(argv[1]);
    char* nomeArquivo = "access_log.txt";
    long num_linhas;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    char **linhas_do_arquivo = lerArquivoParaArray(nomeArquivo, &num_linhas);
    if (linhas_do_arquivo == NULL) {
        return 1;
    }

    pthread_t threads[NUM_THREADS];
    Stats tdata[NUM_THREADS];

    long slice_size = num_linhas / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        tdata[i].lines = linhas_do_arquivo;
        tdata[i].inicio = i * slice_size;
        tdata[i].fim = (i == NUM_THREADS - 1) ? num_linhas : (i + 1) * slice_size;
        
        if (pthread_create(&threads[i], NULL, funcaoThreads, &tdata[i]) != 0) {
            perror("pthread_create");
            exit(1);
        }
    } 

    // Estatísticas
    long long total_bytes_global = 0;
    long long total_errors404_global = 0;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total_bytes_global += tdata[i].total_bytes;
        total_errors404_global += tdata[i].errors404;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Total de Bytes = %lld Bytes\n", total_bytes_global);
    printf("Total de códigos 404 = %lld\n", total_errors404_global);
    printf("Tempo de execução: %.9f segundos\n", time_spent);

    for (long i = 0; i < num_linhas; i++) {
        free(linhas_do_arquivo[i]);
    }
    free(linhas_do_arquivo);

    return 0;
}
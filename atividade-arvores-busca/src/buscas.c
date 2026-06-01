#include <stdlib.h>
#include <time.h>
#include "buscas.h"

int buscaSequencial(int vetor[], int tamanho, int valor, int *comparacoes) {
    *comparacoes = 0;
    for (int i = 0; i < tamanho; i++) {
        (*comparacoes)++;
        if (vetor[i] == valor) return i;
    }
    return -1;
}

int buscaBinaria(int vetor[], int tamanho, int valor, int *comparacoes) {
    *comparacoes = 0;
    int esq = 0, dir = tamanho - 1;
    while (esq <= dir) {
        (*comparacoes)++;
        int meio = esq + (dir - esq) / 2;
        if (vetor[meio] == valor) return meio;
        else if (vetor[meio] < valor) esq = meio + 1;
        else dir = meio - 1;
    }
    return -1;
}

static int comparaInt(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void ordenarVetor(int vetor[], int tamanho) {
    qsort(vetor, tamanho, sizeof(int), comparaInt);
}

void embaralharVetor(int vetor[], int tamanho) {
    srand((unsigned int)time(NULL));
    for (int i = tamanho - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = vetor[i];
        vetor[i] = vetor[j];
        vetor[j] = tmp;
    }
}

#ifndef BUSCAS_H
#define BUSCAS_H

/* Busca sequencial em vetor (nao precisa estar ordenado) */
int buscaSequencial(int vetor[], int tamanho, int valor, int *comparacoes);

/* Busca binaria em vetor ordenado */
int buscaBinaria(int vetor[], int tamanho, int valor, int *comparacoes);

/* Ordena o vetor usando qsort da stdlib */
void ordenarVetor(int vetor[], int tamanho);

/* Embaralha o vetor usando Fisher-Yates */
void embaralharVetor(int vetor[], int tamanho);

#endif

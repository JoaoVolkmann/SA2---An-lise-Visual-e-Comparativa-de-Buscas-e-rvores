#ifndef ARVORE_H
#define ARVORE_H

typedef struct No {
    int valor;
    struct No *esquerda;
    struct No *direita;
} No;

No* inserirNaArvore(No *raiz, int valor);
int buscarNaArvore(No *raiz, int valor, int *comparacoes);
int alturaArvore(No *raiz);
void liberarArvore(No *raiz);
void exportarDOT(No *raiz, const char *arquivo);

#endif

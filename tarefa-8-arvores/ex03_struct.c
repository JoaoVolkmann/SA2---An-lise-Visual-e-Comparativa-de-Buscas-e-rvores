#include <stdio.h>

/* Exercício 3 — Struct do nó de uma árvore binária */

typedef struct No {
    int valor;
    struct No *esquerda;
    struct No *direita;
} No;

int main(void) {
    printf("Struct 'No' definida com sucesso.\n");
    printf("Campos: valor (int), esquerda (No*), direita (No*)\n");
    return 0;
}

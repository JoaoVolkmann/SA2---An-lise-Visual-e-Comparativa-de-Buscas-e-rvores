#include <stdio.h>
#include <stdlib.h>
#include "arvore.h"

No* inserirNaArvore(No *raiz, int valor) {
    No *novo = (No*)malloc(sizeof(No));
    if (!novo) {
        fprintf(stderr, "Erro: falha ao alocar memoria para o no.\n");
        exit(1);
    }
    novo->valor = valor;
    novo->esquerda = NULL;
    novo->direita = NULL;

    if (raiz == NULL) return novo;

    /* Insercao iterativa para evitar estouro de pilha em arvores degeneradas */
    No *atual = raiz;
    while (1) {
        if (valor < atual->valor) {
            if (atual->esquerda == NULL) { atual->esquerda = novo; break; }
            atual = atual->esquerda;
        } else if (valor > atual->valor) {
            if (atual->direita == NULL) { atual->direita = novo; break; }
            atual = atual->direita;
        } else {
            free(novo); /* duplicado: descarta */
            break;
        }
    }
    return raiz;
}

int buscarNaArvore(No *raiz, int valor, int *comparacoes) {
    *comparacoes = 0;
    No *atual = raiz;
    while (atual != NULL) {
        (*comparacoes)++;
        if (valor == atual->valor) return 1;
        else if (valor < atual->valor) atual = atual->esquerda;
        else atual = atual->direita;
    }
    return 0;
}

/* Altura iterativa usando BFS nivel a nivel */
int alturaArvore(No *raiz) {
    if (raiz == NULL) return 0;

    /* Para arvore degenerada (lista), BFS usa O(1) nos por nivel -> seguro.
       Para arvore balanceada, o nivel mais largo tem n/2 nos.
       Usamos fila dinamica crescente. */
    size_t capacidade = 1024;
    No **fila = (No**)malloc(capacidade * sizeof(No*));
    if (!fila) return -1;

    size_t ini = 0, fim = 0;
    int altura = 0;

    fila[fim++] = raiz;

    while (ini < fim) {
        size_t nos_nivel = fim - ini;
        altura++;
        for (size_t i = 0; i < nos_nivel; i++) {
            No *no = fila[ini++];
            /* Realloca se necessario */
            if (fim + 2 >= capacidade) {
                capacidade *= 2;
                No **tmp = (No**)realloc(fila, capacidade * sizeof(No*));
                if (!tmp) { free(fila); return -1; }
                fila = tmp;
            }
            if (no->esquerda) fila[fim++] = no->esquerda;
            if (no->direita)  fila[fim++] = no->direita;
        }
    }

    free(fila);
    return altura;
}

/* Liberacao iterativa usando pilha dinamica */
void liberarArvore(No *raiz) {
    if (raiz == NULL) return;

    size_t capacidade = 1024;
    No **pilha = (No**)malloc(capacidade * sizeof(No*));
    if (!pilha) return;

    size_t topo = 0;
    pilha[topo++] = raiz;

    while (topo > 0) {
        No *no = pilha[--topo];
        if (no->esquerda) {
            if (topo + 1 >= capacidade) {
                capacidade *= 2;
                No **tmp = (No**)realloc(pilha, capacidade * sizeof(No*));
                if (!tmp) { free(pilha); return; }
                pilha = tmp;
            }
            pilha[topo++] = no->esquerda;
        }
        if (no->direita) {
            if (topo + 1 >= capacidade) {
                capacidade *= 2;
                No **tmp = (No**)realloc(pilha, capacidade * sizeof(No*));
                if (!tmp) { free(pilha); return; }
                pilha = tmp;
            }
            pilha[topo++] = no->direita;
        }
        free(no);
    }
    free(pilha);
}

/* Exporta a arvore no formato DOT para visualizacao com Graphviz */
static void exportarNos(No *raiz, FILE *f) {
    if (raiz == NULL) return;
    if (raiz->esquerda) {
        fprintf(f, "    %d -> %d;\n", raiz->valor, raiz->esquerda->valor);
        exportarNos(raiz->esquerda, f);
    }
    if (raiz->direita) {
        fprintf(f, "    %d -> %d;\n", raiz->valor, raiz->direita->valor);
        exportarNos(raiz->direita, f);
    }
}

void exportarDOT(No *raiz, const char *arquivo) {
    FILE *f = fopen(arquivo, "w");
    if (!f) {
        fprintf(stderr, "Erro ao abrir arquivo DOT: %s\n", arquivo);
        return;
    }
    fprintf(f, "digraph Arvore {\n");
    exportarNos(raiz, f);
    fprintf(f, "}\n");
    fclose(f);
}

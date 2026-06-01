#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "arvore.h"
#include "buscas.h"

/* ------------------------------------------------------------------ */
/* Utilitarios                                                          */
/* ------------------------------------------------------------------ */

/* Preenche vetor com valores 1..n */
static void preencherVetor(int *v, long n) {
    for (long i = 0; i < n; i++) v[i] = (int)(i + 1);
}

/* Cabecalho da tabela CSV */
static void imprimirCabecalhoCSV(FILE *csv) {
    fprintf(csv,
        "tamanho,tipo,valor_buscado,"
        "seq_comp,seq_encontrado,"
        "bin_comp,bin_encontrado,"
        "arv_comp,arv_encontrado,"
        "altura_arvore,"
        "tempo_seq,tempo_bin,tempo_arv\n");
}

/* ------------------------------------------------------------------ */
/* Experimento principal                                                */
/* ------------------------------------------------------------------ */

typedef struct {
    long   tamanho;
    int    ordenado;   /* 1 = ordenado, 0 = embaralhado */
} ConfigExp;

static void rodarExperimento(ConfigExp cfg, FILE *csv, FILE *resumo) {
    const char *tipo = cfg.ordenado ? "Ordenados" : "Embaralhados";
    long n = cfg.tamanho;

    printf("\n=== Experimento: n=%ld, dados %s ===\n", n, tipo);
    fprintf(resumo, "\n=== Experimento: n=%ld, dados %s ===\n", n, tipo);

    /* ---- Alocacao ---- */
    int *vetor = (int*)malloc((size_t)n * sizeof(int));
    if (!vetor) {
        fprintf(stderr, "Erro: nao foi possivel alocar vetor de %ld elementos.\n", n);
        return;
    }

    /* Preenche 1..n */
    preencherVetor(vetor, n);

    /* Embaralha se necessario (usaremos este vetor para montar a arvore) */
    if (!cfg.ordenado) embaralharVetor(vetor, (int)n);

    /* ---- Monta a arvore ---- */
    No *raiz = NULL;
    printf("  Inserindo %ld nos na arvore...\n", n);
    for (long i = 0; i < n; i++) {
        raiz = inserirNaArvore(raiz, vetor[i]);
    }

    int altura = -1;
    /* Para dados ordenados, a arvore degenera em lista: altura == n.
       Evitamos BFS de n nos para economizar memoria e tempo.
       Para dados embaralhados, calculamos normalmente. */
    if (cfg.ordenado) {
        altura = (n <= 2000000000L) ? (int)n : -1;
    } else {
        altura = alturaArvore(raiz);
    }

    printf("  Altura da arvore: %d\n", altura);
    fprintf(resumo, "  Altura da arvore: %d\n", altura);

    /* Exporta DOT apenas para n=100 (visualizacao pratica) */
    if (n == 100) {
        char nome[64];
        snprintf(nome, sizeof(nome), "resultados/arvore_%s.dot", tipo);
        exportarDOT(raiz, nome);
        printf("  Arquivo DOT gerado: %s\n", nome);
    }

    /* ---- Ordena vetor para busca binaria ---- */
    /* O vetor ja esta preenchido; se foi embaralhado para a arvore,
       precisamos de uma copia ordenada para busca binaria.          */
    int *vetorOrdenado = (int*)malloc((size_t)n * sizeof(int));
    if (!vetorOrdenado) {
        fprintf(stderr, "Erro: nao foi possivel alocar vetor ordenado.\n");
        liberarArvore(raiz);
        free(vetor);
        return;
    }
    /* Copia e ordena */
    memcpy(vetorOrdenado, vetor, (size_t)n * sizeof(int));
    ordenarVetor(vetorOrdenado, (int)n);

    /* ---- Valores de busca ---- */
    /* inicio, meio, fim, inexistente */
    int valores[4];
    valores[0] = 1;                       /* inicio */
    valores[1] = (int)(n / 2);            /* meio   */
    valores[2] = (int)n;                  /* fim    */
    valores[3] = (int)n + 999;            /* inexistente */

    const char *labels[4] = {"inicio", "meio", "fim", "inexistente"};

    for (int v = 0; v < 4; v++) {
        int alvo = valores[v];
        int cSeq = 0, cBin = 0, cArv = 0;
        int rSeq, rBin, rArv;
        clock_t t0, t1;
        double tSeq, tBin, tArv;

        /* Busca sequencial (no vetor embaralhado/original) */
        t0 = clock();
        rSeq = buscaSequencial(vetor, (int)n, alvo, &cSeq);
        t1 = clock();
        tSeq = (double)(t1 - t0) / CLOCKS_PER_SEC;

        /* Busca binaria (no vetor ordenado) */
        t0 = clock();
        rBin = buscaBinaria(vetorOrdenado, (int)n, alvo, &cBin);
        t1 = clock();
        tBin = (double)(t1 - t0) / CLOCKS_PER_SEC;

        /* Busca na arvore */
        t0 = clock();
        rArv = buscarNaArvore(raiz, alvo, &cArv);
        t1 = clock();
        tArv = (double)(t1 - t0) / CLOCKS_PER_SEC;

        /* Impressao no terminal */
        printf("  Busca [%s=%d]: Seq=%d comp(enc=%d) | Bin=%d comp(enc=%d) | Arv=%d comp(enc=%d)\n",
               labels[v], alvo, cSeq, rSeq >= 0 ? 1 : 0,
               cBin, rBin >= 0 ? 1 : 0,
               cArv, rArv);

        fprintf(resumo,
               "  Busca [%s=%d]: Seq=%d comp | Bin=%d comp | Arv=%d comp | Altura=%d\n",
               labels[v], alvo, cSeq, cBin, cArv, altura);

        /* CSV */
        fprintf(csv, "%ld,%s,%d,%d,%d,%d,%d,%d,%d,%d,%.6f,%.6f,%.6f\n",
                n, tipo, alvo,
                cSeq, (rSeq >= 0 ? 1 : 0),
                cBin, (rBin >= 0 ? 1 : 0),
                cArv, rArv,
                altura,
                tSeq, tBin, tArv);
    }

    liberarArvore(raiz);
    free(vetor);
    free(vetorOrdenado);
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */

int main(void) {
    FILE *csv = fopen("resultados/resultados.csv", "w");
    if (!csv) { perror("fopen resultados.csv"); return 1; }

    FILE *resumo = fopen("resultados/resumo.txt", "w");
    if (!resumo) { perror("fopen resumo.txt"); fclose(csv); return 1; }

    imprimirCabecalhoCSV(csv);

    /* Experimentos conforme enunciado */
    ConfigExp experimentos[] = {
        {100,     1},   /* 1 - ordenado    */
        {100,     0},   /* 2 - embaralhado */
        {10000,   1},   /* 3 - ordenado    */
        {10000,   0},   /* 4 - embaralhado */
        {100000,  1},   /* 5 - ordenado    */
        {100000,  0},   /* 6 - embaralhado */
        /* Experimentos com 1.000.000+ requerem hardware dedicado.
           Descomente abaixo para executar em maquina local: */
        /* {1000000,     1}, */
        /* {1000000,     0}, */
        /* {1000000000L, 1}, */
        /* {1000000000L, 0}, */
    };

    int total = (int)(sizeof(experimentos) / sizeof(experimentos[0]));

    printf("=================================================\n");
    printf("  Experimento: Comparacao de Estrategias de Busca\n");
    printf("=================================================\n");
    printf("Experimentos com 1M+ nos desativados por padrao (hardware dedicado).\n");
    printf("Para ativa-los, descomente em main.c e recompile.\n");

    for (int i = 0; i < total; i++) {
        rodarExperimento(experimentos[i], csv, resumo);
    }

    fclose(csv);
    fclose(resumo);

    printf("\nResultados salvos em resultados/resultados.csv e resultados/resumo.txt\n");
    return 0;
}

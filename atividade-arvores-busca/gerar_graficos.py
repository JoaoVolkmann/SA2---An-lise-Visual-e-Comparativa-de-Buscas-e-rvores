#!/usr/bin/env python3
"""
Gera graficos a partir dos resultados do experimento de busca.
"""
import csv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

CSV_PATH = "resultados/resultados.csv"

# ------------------------------------------------------------------ #
# Leitura dos dados                                                    #
# ------------------------------------------------------------------ #

rows = []
with open(CSV_PATH, newline='') as f:
    reader = csv.DictReader(f)
    for row in reader:
        rows.append({
            'n':        int(row['tamanho']),
            'tipo':     row['tipo'],
            'valor':    int(row['valor_buscado']),
            'seq':      int(row['seq_comp']),
            'bin':      int(row['bin_comp']),
            'arv':      int(row['arv_comp']),
            'altura':   int(row['altura_arvore']),
        })

# Paleta de cores
C_SEQ = '#E74C3C'
C_BIN = '#2ECC71'
C_ARV = '#3498DB'
C_ORD = '#E67E22'
C_EMB = '#9B59B6'

plt.rcParams.update({
    'font.family': 'DejaVu Sans',
    'axes.spines.top': False,
    'axes.spines.right': False,
    'figure.dpi': 150,
})

# ------------------------------------------------------------------ #
# Grafico 1 — Comparacoes por algoritmo (valores do meio, por tamanho)#
# ------------------------------------------------------------------ #

fig, axes = plt.subplots(1, 2, figsize=(14, 6))
fig.suptitle('Comparações por Algoritmo de Busca', fontsize=14, fontweight='bold', y=1.02)

for ax_idx, tipo in enumerate(['Ordenados', 'Embaralhados']):
    ax = axes[ax_idx]
    subset = [r for r in rows if r['tipo'] == tipo]

    # Agrupa pelo tamanho, pega o valor do "meio"
    tamanhos = sorted(set(r['n'] for r in subset))
    seq_vals, bin_vals, arv_vals = [], [], []

    for n in tamanhos:
        # Pega a linha cujo valor buscado e aproximadamente n/2
        meio = n // 2
        candidatos = [r for r in subset if r['n'] == n]
        # Escolhe o valor mais proximo do meio
        linha = min(candidatos, key=lambda r: abs(r['valor'] - meio))
        seq_vals.append(linha['seq'])
        bin_vals.append(linha['bin'])
        arv_vals.append(linha['arv'])

    x = np.arange(len(tamanhos))
    width = 0.25

    bars1 = ax.bar(x - width, seq_vals, width, label='Sequencial', color=C_SEQ, alpha=0.85)
    bars2 = ax.bar(x,         bin_vals, width, label='Binária',    color=C_BIN, alpha=0.85)
    bars3 = ax.bar(x + width, arv_vals, width, label='Árvore BST', color=C_ARV, alpha=0.85)

    ax.set_title(f'Dados {tipo}', fontweight='bold')
    ax.set_xlabel('Tamanho da entrada (n)')
    ax.set_ylabel('Número de comparações')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{n:,}' for n in tamanhos])
    ax.legend()
    ax.set_yscale('log')
    ax.yaxis.grid(True, alpha=0.3)

    # Anota valores
    for bar in [*bars1, *bars2, *bars3]:
        h = bar.get_height()
        ax.annotate(f'{int(h):,}',
                    xy=(bar.get_x() + bar.get_width() / 2, h),
                    xytext=(0, 3), textcoords='offset points',
                    ha='center', va='bottom', fontsize=7)

plt.tight_layout()
plt.savefig('resultados/grafico_comparacoes.png', bbox_inches='tight')
print("Grafico 1 salvo: resultados/grafico_comparacoes.png")

# ------------------------------------------------------------------ #
# Grafico 2 — Altura da arvore: ordenado vs embaralhado               #
# ------------------------------------------------------------------ #

fig2, ax2 = plt.subplots(figsize=(10, 6))
ax2.set_title('Altura da Árvore Binária de Busca\nOrdenado vs Embaralhado', fontweight='bold')

tamanhos = sorted(set(r['n'] for r in rows))
alturas_ord = []
alturas_emb = []

for n in tamanhos:
    linhas_ord = [r for r in rows if r['n'] == n and r['tipo'] == 'Ordenados']
    linhas_emb = [r for r in rows if r['n'] == n and r['tipo'] == 'Embaralhados']
    if linhas_ord: alturas_ord.append(linhas_ord[0]['altura'])
    if linhas_emb: alturas_emb.append(linhas_emb[0]['altura'])

x = np.arange(len(tamanhos))
width = 0.35

b1 = ax2.bar(x - width/2, alturas_ord, width, label='Dados Ordenados (degenerada)',   color=C_ORD, alpha=0.85)
b2 = ax2.bar(x + width/2, alturas_emb, width, label='Dados Embaralhados (balanceada)', color=C_EMB, alpha=0.85)

ax2.set_xlabel('Tamanho da entrada (n)')
ax2.set_ylabel('Altura da árvore')
ax2.set_xticks(x)
ax2.set_xticklabels([f'{n:,}' for n in tamanhos])
ax2.legend()
ax2.set_yscale('log')
ax2.yaxis.grid(True, alpha=0.3)
ax2.spines['top'].set_visible(False)
ax2.spines['right'].set_visible(False)

for bar in [*b1, *b2]:
    h = bar.get_height()
    ax2.annotate(f'{int(h):,}',
                 xy=(bar.get_x() + bar.get_width() / 2, h),
                 xytext=(0, 3), textcoords='offset points',
                 ha='center', va='bottom', fontsize=8)

# Linha de referencia log2(n)
log2_vals = [np.log2(n) for n in tamanhos]
ax2.plot(x, log2_vals, 'k--', linewidth=1.5, label='log₂(n) — ideal')
ax2.legend()

plt.tight_layout()
plt.savefig('resultados/grafico_altura_arvore.png', bbox_inches='tight')
print("Grafico 2 salvo: resultados/grafico_altura_arvore.png")

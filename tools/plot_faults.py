"""Gera um grafico de barras das faltas por algoritmo a partir de reports/comparacao.csv.
Uso: python tools/plot_faults.py  (requer matplotlib)."""
import csv
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

rows = list(csv.DictReader(open("reports/comparacao.csv", encoding="utf-8")))

alg = [r["algoritmo"] for r in rows]

faltas = [int(r["faltas"]) for r in rows]

plt.figure(figsize=(6, 4))

bars = plt.bar(alg, faltas, color=["#C0504D", "#4472C4", "#2E7D32"])

for b, v in zip(bars, faltas):
    plt.text(b.get_x() + b.get_width() / 2, v, str(v), ha="center", va="bottom", fontweight="bold")

plt.ylabel("faltas de pagina")
plt.title("Faltas por algoritmo de substituicao (mesma carga)")
plt.tight_layout()
plt.savefig("reports/comparacao.png", dpi=150)

print("salvo: reports/comparacao.png")

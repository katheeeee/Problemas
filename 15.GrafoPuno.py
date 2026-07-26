from collections import defaultdict, deque
import sys
import time
import numpy as np


class GrafoPuno:
    CIUDADES = {
        0: 'Puno', 1: 'Juliaca', 2: 'Ilave', 3: 'Desaguadero', 4: 'Yunguyo',
        5: 'Juli', 6: 'Lampa', 7: 'Azangaro', 8: 'Huancane', 9: 'Moho',
        10: 'Putina', 11: 'Ayaviri', 12: 'Macusani', 13: 'Sandia',
    }
    RIESGO = {
        0: 'bajo', 1: 'bajo', 2: 'bajo', 3: 'bajo', 4: 'medio', 5: 'bajo',
        6: 'medio', 7: 'medio', 8: 'alto', 9: 'alto', 10: 'alto',
        11: 'medio', 12: 'alto', 13: 'alto',
    }

    def __init__(self, n: int):
        if n <= 0:
            raise ValueError("n debe ser positivo")
        self.n = n
        self.adj: dict[int, list[tuple[int, float]]] = defaultdict(list)
        self.naristas = 0

    def _validar_vertice(self, u: int):
        if not (0 <= u < self.n):
            raise IndexError(f"Vertice fuera de rango: {u}")

    def existe_arista(self, u: int, v: int) -> bool:
        self._validar_vertice(u)
        self._validar_vertice(v)
        return any(dest == v for dest, _ in self.adj[u])

    def agregar_arista(self, u: int, v: int, peso: float):
        self._validar_vertice(u)
        self._validar_vertice(v)
        if u == v:
            raise ValueError(f"No se permiten bucles: ({u},{u})")
        if self.existe_arista(u, v):
            raise ValueError(f"Arista duplicada: ({u},{v})")
        self.adj[u].append((v, peso))
        self.adj[v].append((u, peso))
        self.naristas += 1

    def vecinos(self, u: int) -> list[tuple[int, float]]:
        self._validar_vertice(u)
        return self.adj[u]

    def grado(self, u: int) -> int:
        self._validar_vertice(u)
        return len(self.adj[u])

    def densidad(self) -> float:
        if self.n < 2:
            return 0.0
        return 2 * self.naristas / (self.n * (self.n - 1))

    def nombre(self, u: int) -> str:
        return self.CIUDADES.get(u, str(u))


def construir_matriz(g: GrafoPuno) -> np.ndarray:
    M = np.full((g.n, g.n), np.inf)
    np.fill_diagonal(M, 0)
    for u in g.adj:
        for v, peso in g.adj[u]:
            M[u][v] = peso
    return M


def comparar_memoria(g: GrafoPuno, matriz: np.ndarray) -> tuple[int, int, float]:
    tam_lista = sys.getsizeof(g.adj) + sum(sys.getsizeof(v) for v in g.adj.values())
    tam_matriz = matriz.nbytes
    razon = tam_matriz / tam_lista if tam_lista else float('inf')
    return tam_lista, tam_matriz, razon


class GrafoDirigidoPuno:
    def __init__(self, n: int):
        if n <= 0:
            raise ValueError("n debe ser positivo")
        self.n = n
        self.adj: dict[int, list[tuple[int, float]]] = defaultdict(list)

    def agregar_arista_dirigida(self, u: int, v: int, peso: float):
        if not (0 <= u < self.n) or not (0 <= v < self.n):
            raise IndexError(f"Vertice fuera de rango: ({u},{v})")
        self.adj[u].append((v, peso))

    def es_alcanzable(self, origen: int, destino: int) -> bool:
        if origen == destino:
            return True
        visitados = {origen}
        pila = [origen]
        while pila:
            u = pila.pop()
            for v, _ in self.adj[u]:
                if v == destino:
                    return True
                if v not in visitados:
                    visitados.add(v)
                    pila.append(v)
        return False


def componentes_conexas(g: GrafoPuno, vertices_excluidos: set[int] | None = None) -> list[list[int]]:
    excluidos = vertices_excluidos or set()
    visitados = set(excluidos)
    componentes = []
    for inicio in range(g.n):
        if inicio in visitados:
            continue
        componente = []
        cola = deque([inicio])
        visitados.add(inicio)
        while cola:
            u = cola.popleft()
            componente.append(u)
            for v, _ in g.adj[u]:
                if v not in visitados and v not in excluidos:
                    visitados.add(v)
                    cola.append(v)
        componentes.append(componente)
    return componentes


def construir_red_vial() -> GrafoPuno:
    g = GrafoPuno(14)
    rutas = [
        (0, 1, 44), (0, 2, 55), (0, 5, 80), (1, 6, 37), (1, 7, 70), (1, 11, 90),
        (2, 3, 50), (2, 4, 45), (3, 4, 25), (5, 4, 60), (7, 8, 95), (7, 10, 110),
        (7, 11, 75), (8, 9, 40), (11, 12, 140), (11, 13, 180),
    ]
    for u, v, p in rutas:
        g.agregar_arista(u, v, p)
    return g


def construir_grafo_candelaria() -> GrafoDirigidoPuno:
    gd = GrafoDirigidoPuno(6)
    restricciones = [(0, 1), (1, 5), (2, 0), (3, 2), (4, 3)]
    for u, v in restricciones:
        gd.agregar_arista_dirigida(u, v, 1)
    return gd


def main():
    g = construir_red_vial()
    print(f"|V|={g.n} |E|={g.naristas} densidad={g.densidad():.4f}")
    for c in range(g.n):
        print(f"  {g.nombre(c):15} grado={g.grado(c)}")

    matriz = construir_matriz(g)
    print("\nMatriz de adyacencia (km, inf = sin ruta directa):")
    print(matriz)

    tam_lista, tam_matriz, razon = comparar_memoria(g, matriz)
    print(f"\nMemoria lista de adyacencia: {tam_lista} bytes")
    print(f"Memoria matriz de adyacencia: {tam_matriz} bytes")
    print(f"Razon matriz/lista: {razon:.3f}x")

    gd = construir_grafo_candelaria()
    print("\n¿Se puede ir del Jr. Tacna (4) a la Plaza de Armas (0)?",
          gd.es_alcanzable(4, 0))
    print("¿Se puede ir de la Plaza de Armas (0) al Jr. Tacna (4)?",
          gd.es_alcanzable(0, 4))

    print("\n=== Red vial COMPLETA (sin lluvias) ===")
    comp_normal = componentes_conexas(g)
    print(f"Componentes conexas: {len(comp_normal)}")
    for c in comp_normal:
        print(f"  {[g.nombre(v) for v in c]}")

    print("\n=== Simulacion: trochas de Macusani(12) y Sandia(13) bloqueadas ===")
    ciudades_aisladas = {12, 13}
    comp_lluvia = componentes_conexas(g, vertices_excluidos=ciudades_aisladas)
    print(f"Componentes conexas restantes: {len(comp_lluvia)}")
    for c in comp_lluvia:
        print(f"  {[g.nombre(v) for v in c]}")
    print(f"Ciudades completamente aisladas: "
          f"{[g.nombre(v) for v in ciudades_aisladas]}")


if __name__ == "__main__":
    main()

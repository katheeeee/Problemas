"""
Practica N 14 - Arboles B+
Consultas de Rango sobre el Catalogo de la Biblioteca Central UNA-PUNO
(Python 3.11+)

Implementa:
  - NodoHojaBP / NodoInternoBP
  - Insercion con split y mantenimiento de la lista enlazada de hojas
  - Busqueda puntual: buscar()
  - Consulta de rango O(log n + k): rango()
  - Recorrido completo del catalogo en O(n): recorrer_todo_el_catalogo()
  - ELIMINACION B+ con redistribucion/fusion de hojas y de nodos internos,
    reparando en todo momento la lista enlazada de hojas
  - Simulacion sobre 80,000 volumenes con consultas de rango reales
  - Prueba de estres: 5,000 eliminaciones aleatorias + verificacion de
    integridad de la lista enlazada (sin ciclos, sin huecos, orden correcto)

Nota de diseno: a diferencia del pseudocodigo de la guia (que solo resuelve
el sub-poblamiento de HOJAS), esta version implementa la reparacion
recursiva tambien para NODOS INTERNOS subpoblados, evitando que el arbol
se corrompa tras muchas eliminaciones seguidas.

Autor: Aico Ramos, Kriss Katherine
"""

from dataclasses import dataclass
import random
import time


# ---------------------------------------------------------------------------
# Modelo de datos
# ---------------------------------------------------------------------------
@dataclass
class Libro:
    titulo: str
    autor: str


class NodoHojaBP:
    __slots__ = ("claves", "libros", "siguiente")

    def __init__(self):
        self.claves: list[str] = []
        self.libros: list[Libro] = []
        self.siguiente: "NodoHojaBP | None" = None


class NodoInternoBP:
    __slots__ = ("claves", "hijos")

    def __init__(self):
        self.claves: list[str] = []          # solo claves guia
        self.hijos: list = []                # NodoHojaBP o NodoInternoBP


# ---------------------------------------------------------------------------
# Arbol B+
# ---------------------------------------------------------------------------
class ArbolBMas:
    """Arbol B+ de orden minimo t. Los datos viven UNICAMENTE en las hojas;
    los nodos internos solo almacenan claves guia."""

    def __init__(self, t: int = 4):
        if t < 2:
            raise ValueError("El orden minimo t debe ser >= 2")
        self.t = t
        self.raiz: NodoHojaBP | NodoInternoBP = NodoHojaBP()

    @property
    def primera_hoja(self) -> NodoHojaBP:
        """Hoja mas a la izquierda; se calcula bajo demanda (siempre
        correcta, incluso tras splits/fusiones que reestructuren el arbol)."""
        nodo = self.raiz
        while isinstance(nodo, NodoInternoBP):
            nodo = nodo.hijos[0]
        return nodo

    # ------------------------------------------------------------------ #
    # Insercion con split
    # ------------------------------------------------------------------ #
    def insertar(self, codigo: str, libro: Libro):
        resultado = self._ins(self.raiz, codigo, libro)
        if resultado:  # la raiz se dividio: crear nueva raiz interna
            clave_subida, nuevo_hermano = resultado
            nueva_raiz = NodoInternoBP()
            nueva_raiz.claves = [clave_subida]
            nueva_raiz.hijos = [self.raiz, nuevo_hermano]
            self.raiz = nueva_raiz

    def _ins(self, nodo, codigo: str, libro: Libro):
        if isinstance(nodo, NodoHojaBP):
            i = 0
            while i < len(nodo.claves) and codigo > nodo.claves[i]:
                i += 1
            if i < len(nodo.claves) and nodo.claves[i] == codigo:
                nodo.libros[i] = libro   # codigo duplicado: actualiza
                return None
            nodo.claves.insert(i, codigo)
            nodo.libros.insert(i, libro)
            if len(nodo.claves) <= 2 * self.t - 1:
                return None
            return self._split_hoja(nodo)
        else:
            i = 0
            while i < len(nodo.claves) and codigo >= nodo.claves[i]:
                i += 1
            resultado = self._ins(nodo.hijos[i], codigo, libro)
            if not resultado:
                return None
            clave_subida, nuevo_hermano = resultado
            nodo.claves.insert(i, clave_subida)
            nodo.hijos.insert(i + 1, nuevo_hermano)
            if len(nodo.claves) <= 2 * self.t - 1:
                return None
            return self._split_interno(nodo)

    def _split_hoja(self, hoja: NodoHojaBP):
        mitad = len(hoja.claves) // 2
        nueva = NodoHojaBP()
        nueva.claves = hoja.claves[mitad:]
        nueva.libros = hoja.libros[mitad:]
        hoja.claves = hoja.claves[:mitad]
        hoja.libros = hoja.libros[:mitad]

        nueva.siguiente = hoja.siguiente   # CRITICO: mantener la lista enlazada
        hoja.siguiente = nueva

        return (nueva.claves[0], nueva)     # la clave guia sube al padre

    def _split_interno(self, nodo: NodoInternoBP):
        mitad = len(nodo.claves) // 2
        clave_media = nodo.claves[mitad]

        nuevo = NodoInternoBP()
        nuevo.claves = nodo.claves[mitad + 1:]
        nuevo.hijos = nodo.hijos[mitad + 1:]

        nodo.claves = nodo.claves[:mitad]
        nodo.hijos = nodo.hijos[:mitad + 1]

        return (clave_media, nuevo)

    # ------------------------------------------------------------------ #
    # Busqueda puntual y consulta de rango O(log n + k)
    # ------------------------------------------------------------------ #
    def _bajar_a_hoja(self, codigo: str) -> NodoHojaBP:
        """Desciende desde la raiz hasta la hoja donde deberia estar codigo."""
        nodo = self.raiz
        while isinstance(nodo, NodoInternoBP):
            i = 0
            while i < len(nodo.claves) and codigo >= nodo.claves[i]:
                i += 1
            nodo = nodo.hijos[i]
        return nodo

    def buscar(self, codigo: str) -> Libro | None:
        hoja = self._bajar_a_hoja(codigo)
        for i, k in enumerate(hoja.claves):
            if k == codigo:
                return hoja.libros[i]
        return None

    def rango(self, codigo_min: str, codigo_max: str) -> list[Libro]:
        """Retorna todos los libros con codigo en [codigo_min, codigo_max].
        Complejidad: O(log n) para llegar a la primera hoja + O(k) para
        recorrer la lista enlazada hasta superar codigo_max."""
        resultados: list[Libro] = []
        hoja = self._bajar_a_hoja(codigo_min)
        while hoja:
            for i, k in enumerate(hoja.claves):
                if codigo_min <= k <= codigo_max:
                    resultados.append(hoja.libros[i])
                elif k > codigo_max:
                    return resultados
            hoja = hoja.siguiente          # la ventaja del B+: sin re-descenso
        return resultados

    def recorrer_todo_el_catalogo(self) -> list[Libro]:
        """Recorre TODAS las hojas en O(n) sin tocar los nodos internos."""
        resultados: list[Libro] = []
        hoja = self.primera_hoja
        while hoja:
            resultados.extend(hoja.libros)
            hoja = hoja.siguiente
        return resultados

    # ------------------------------------------------------------------ #
    # Eliminacion B+ (hojas + nodos internos, con reparacion recursiva)
    # ------------------------------------------------------------------ #
    def eliminar(self, codigo: str):
        if self.buscar(codigo) is None:
            raise KeyError(f"Codigo no encontrado: {codigo}")
        self._eliminar_rec(self.raiz, codigo)
        # Si la raiz interna se quedo sin claves, su unico hijo pasa a ser
        # la nueva raiz (reduce la altura del arbol en uno).
        if isinstance(self.raiz, NodoInternoBP) and len(self.raiz.claves) == 0:
            self.raiz = self.raiz.hijos[0]

    def _eliminar_rec(self, nodo, codigo: str) -> bool:
        """Elimina codigo del subarbol de nodo (modificado in-place).
        Retorna True si nodo quedo con menos del minimo de claves (t-1),
        es decir, si el LLAMADOR debe repararlo."""
        t = self.t
        if isinstance(nodo, NodoHojaBP):
            idx = nodo.claves.index(codigo)
            nodo.claves.pop(idx)
            nodo.libros.pop(idx)
            return len(nodo.claves) < t - 1
        else:
            i = 0
            while i < len(nodo.claves) and codigo >= nodo.claves[i]:
                i += 1
            underflow = self._eliminar_rec(nodo.hijos[i], codigo)
            if underflow:
                self._reparar_hijo(nodo, i)
            return len(nodo.claves) < t - 1

    def _reparar_hijo(self, padre, i: int):
        """Repara padre.hijos[i], que quedo con menos del minimo de claves,
        mediante redistribucion (borrow) desde un hermano, o fusion."""
        t = self.t
        hijo = padre.hijos[i]
        herm_izq = padre.hijos[i - 1] if i > 0 else None
        herm_der = padre.hijos[i + 1] if i < len(padre.hijos) - 1 else None

        if isinstance(hijo, NodoHojaBP):
            # --- Redistribuir desde el hermano derecho ---
            if herm_der and len(herm_der.claves) > t - 1:
                hijo.claves.append(herm_der.claves.pop(0))
                hijo.libros.append(herm_der.libros.pop(0))
                padre.claves[i] = herm_der.claves[0]
                return
            # --- Redistribuir desde el hermano izquierdo ---
            if herm_izq and len(herm_izq.claves) > t - 1:
                hijo.claves.insert(0, herm_izq.claves.pop())
                hijo.libros.insert(0, herm_izq.libros.pop())
                padre.claves[i - 1] = hijo.claves[0]
                return
            # --- Fusionar (sin posibilidad de redistribuir) ---
            if herm_der:
                hijo.claves.extend(herm_der.claves)
                hijo.libros.extend(herm_der.libros)
                hijo.siguiente = herm_der.siguiente   # reparar la lista
                padre.claves.pop(i)
                padre.hijos.pop(i + 1)
            else:
                herm_izq.claves.extend(hijo.claves)
                herm_izq.libros.extend(hijo.libros)
                herm_izq.siguiente = hijo.siguiente   # reparar la lista
                padre.claves.pop(i - 1)
                padre.hijos.pop(i)
        else:
            # Nodo interno subpoblado: redistribuir o fusionar,
            # bajando/subiendo la clave guia correspondiente del padre.
            if herm_der and len(herm_der.claves) > t - 1:
                hijo.claves.append(padre.claves[i])
                padre.claves[i] = herm_der.claves.pop(0)
                hijo.hijos.append(herm_der.hijos.pop(0))
                return
            if herm_izq and len(herm_izq.claves) > t - 1:
                hijo.claves.insert(0, padre.claves[i - 1])
                padre.claves[i - 1] = herm_izq.claves.pop()
                hijo.hijos.insert(0, herm_izq.hijos.pop())
                return
            if herm_der:
                hijo.claves.append(padre.claves[i])
                hijo.claves.extend(herm_der.claves)
                hijo.hijos.extend(herm_der.hijos)
                padre.claves.pop(i)
                padre.hijos.pop(i + 1)
            else:
                herm_izq.claves.append(padre.claves[i - 1])
                herm_izq.claves.extend(hijo.claves)
                herm_izq.hijos.extend(hijo.hijos)
                padre.claves.pop(i - 1)
                padre.hijos.pop(i)

    # ------------------------------------------------------------------ #
    # Verificacion de integridad (usada por la prueba de estres)
    # ------------------------------------------------------------------ #
    def verificar_lista_enlazada(self, n_max_hojas: int = 10_000_000) -> bool:
        """Recorre la lista enlazada de hojas y confirma que:
        (a) termina en un numero finito de pasos (sin ciclos),
        (b) las claves quedan en orden estrictamente ascendente."""
        vistos = set()
        anterior = None
        hoja = self.primera_hoja
        pasos = 0
        while hoja:
            if id(hoja) in vistos:
                return False  # ciclo detectado
            vistos.add(id(hoja))
            for k in hoja.claves:
                if anterior is not None and k <= anterior:
                    return False  # orden roto
                anterior = k
            hoja = hoja.siguiente
            pasos += 1
            if pasos > n_max_hojas:
                return False
        return True


# ---------------------------------------------------------------------------
# Simulacion: consultas de rango sobre el catalogo completo (Actividad 5)
# ---------------------------------------------------------------------------
def generar_codigos(cantidad: int, semilla: int = 42) -> list[str]:
    rnd = random.Random(semilla)
    codigos = sorted(set(
        f"{rnd.randint(0, 999):03d}.{rnd.randint(0, 999):03d}"
        for _ in range(int(cantidad * 1.0625))
    ))[:cantidad]
    return codigos


def simular_catalogo(n_libros: int = 80_000, t: int = 50, semilla: int = 42):
    rnd = random.Random(semilla)
    catalogo_bp = ArbolBMas(t=t)

    codigos = generar_codigos(n_libros, semilla)
    barajados = codigos[:]
    rnd.shuffle(barajados)

    print(f"Indexando {len(codigos)} volumenes (t={t})...")
    for i, cod in enumerate(barajados):
        catalogo_bp.insertar(cod, Libro(titulo=f"Obra {i}", autor=f"Autor {i % 500}"))
    print("Catalogo indexado.")

    # Consulta real: seccion 004 (Ciencias de la Computacion) completa
    t0 = time.perf_counter()
    resultados = catalogo_bp.rango("004.000", "004.999")
    ms = (time.perf_counter() - t0) * 1000
    print(f"Seccion 004 (Computacion): {len(resultados)} libros encontrados en {ms:.3f}ms")

    # Consulta de un rango mas amplio: todo el rango 000-099
    t0 = time.perf_counter()
    resultados2 = catalogo_bp.rango("000.000", "099.999")
    ms2 = (time.perf_counter() - t0) * 1000
    print(f"Rango 000-099: {len(resultados2)} libros encontrados en {ms2:.3f}ms")

    print(f"Lista enlazada de hojas consistente: {catalogo_bp.verificar_lista_enlazada()}")

    return catalogo_bp, codigos


def prueba_de_estres(catalogo_bp: ArbolBMas, codigos: list[str],
                      n_eliminaciones: int = 5_000, semilla: int = 7):
    """5,000 eliminaciones aleatorias + consultas de rango repetidas,
    verificando que la lista enlazada de hojas permanece consistente
    (sin ciclos, sin huecos, orden correcto) via recorrer_todo_el_catalogo()."""
    rnd = random.Random(semilla)
    a_eliminar = rnd.sample(codigos, n_eliminaciones)
    restantes_esperados = set(codigos) - set(a_eliminar)

    for cod in a_eliminar:
        catalogo_bp.eliminar(cod)

    # Consultas de rango repetidas tras las eliminaciones
    for _ in range(20):
        catalogo_bp.rango("004.000", "004.999")
        catalogo_bp.rango("050.000", "050.999")

    catalogo_final = catalogo_bp.recorrer_todo_el_catalogo()
    claves_finales = [lib.titulo for lib in catalogo_final]  # solo para tamano

    lista_ok = catalogo_bp.verificar_lista_enlazada()
    # Reconstruir claves reales recorriendo la lista (no solo los libros)
    claves_reales = []
    hoja = catalogo_bp.primera_hoja
    while hoja:
        claves_reales.extend(hoja.claves)
        hoja = hoja.siguiente

    sin_huecos = set(claves_reales) == restantes_esperados
    orden_correcto = claves_reales == sorted(claves_reales)

    print("\n=== PRUEBA DE ESTRES (Arbol B+) ===")
    print(f"Eliminaciones realizadas: {len(a_eliminar)}")
    print(f"Lista enlazada sin ciclos y ordenada: {lista_ok}")
    print(f"Catalogo sin huecos (todo lo esperado esta presente): {sin_huecos}")
    print(f"Orden ascendente verificado end-to-end: {orden_correcto}")
    print(f"Total de registros tras la prueba: {len(claves_reales)} "
          f"(esperado: {len(restantes_esperados)})")


if __name__ == "__main__":
    print("=== PRUEBA ARBOL B+ - CATALOGO BIBLIOTECA CENTRAL UNA-PUNO ===\n")
    catalogo_bp, codigos = simular_catalogo(n_libros=80_000, t=50)
    prueba_de_estres(catalogo_bp, codigos, n_eliminaciones=5_000)

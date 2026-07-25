"""
Practica N 13 - Arboles B
Indexacion del Catalogo de la Biblioteca Central UNA-PUNO (Python 3.11+)

Implementa:
  - NodoB / Libro
  - Busqueda
  - Insercion con split (incluye caso de raiz llena)
  - Eliminacion con los 3 casos (hoja, nodo interno, redistribucion/fusion)
  - Simulacion del catalogo: 80,000 codigos, prestamos y devoluciones
  - Recorrido in-order y verificador de propiedades (para la prueba de estres)

Autor: Aico Ramos, Kriss Katherine
"""

from dataclasses import dataclass
import random


# ---------------------------------------------------------------------------
# Modelo de datos
# ---------------------------------------------------------------------------
@dataclass
class Libro:
    codigo_topo: str          # ej. '004.678 M23'
    titulo: str
    autor: str
    disponible: bool = True


class NodoB:
    __slots__ = ("claves", "libros", "hijos", "es_hoja")

    def __init__(self, es_hoja: bool = True):
        self.claves: list[str] = []
        self.libros: list[Libro] = []
        self.hijos: list["NodoB"] = []
        self.es_hoja: bool = es_hoja


# ---------------------------------------------------------------------------
# Arbol B
# ---------------------------------------------------------------------------
class ArbolBBiblioteca:
    """Arbol B de orden minimo t (cada nodo tiene entre t-1 y 2t-1 claves,
    salvo la raiz, que puede tener entre 1 y 2t-1)."""

    def __init__(self, t: int = 2):
        if t < 2:
            raise ValueError("El orden minimo t debe ser >= 2")
        self.t = t
        self.raiz = NodoB(es_hoja=True)
        self._n = 0  # cantidad de claves indexadas (informativo)

    # ------------------------------------------------------------------ #
    # Busqueda
    # ------------------------------------------------------------------ #
    def buscar(self, codigo: str):
        """Retorna el objeto Libro si el codigo existe, o None."""
        resultado = self._buscar(self.raiz, codigo)
        return resultado[0].libros[resultado[1]] if resultado else None

    def _buscar(self, nodo: NodoB, codigo: str):
        i = 0
        while i < len(nodo.claves) and codigo > nodo.claves[i]:
            i += 1
        if i < len(nodo.claves) and codigo == nodo.claves[i]:
            return (nodo, i)
        if nodo.es_hoja:
            return None
        return self._buscar(nodo.hijos[i], codigo)

    # ------------------------------------------------------------------ #
    # Insercion con split
    # ------------------------------------------------------------------ #
    def insertar(self, codigo: str, libro: Libro):
        r = self.raiz
        if len(r.claves) == 2 * self.t - 1:
            s = NodoB(es_hoja=False)
            s.hijos.append(self.raiz)
            self._split(s, 0, r)
            self.raiz = s
        self._ins_no_lleno(self.raiz, codigo, libro)
        self._n += 1

    def _split(self, padre: NodoB, i: int, y: NodoB):
        """Divide y en dos, sube la clave media (indice t-1) al padre."""
        t = self.t
        z = NodoB(es_hoja=y.es_hoja)
        z.claves = y.claves[t:]
        z.libros = y.libros[t:]
        if not y.es_hoja:
            z.hijos = y.hijos[t:]

        clave_media = y.claves[t - 1]
        libro_medio = y.libros[t - 1]

        y.claves = y.claves[: t - 1]
        y.libros = y.libros[: t - 1]
        if not y.es_hoja:
            y.hijos = y.hijos[:t]

        padre.hijos.insert(i + 1, z)
        padre.claves.insert(i, clave_media)
        padre.libros.insert(i, libro_medio)

    def _ins_no_lleno(self, nodo: NodoB, codigo: str, libro: Libro):
        i = len(nodo.claves) - 1
        if nodo.es_hoja:
            nodo.claves.append("")
            nodo.libros.append(None)
            while i >= 0 and codigo < nodo.claves[i]:
                nodo.claves[i + 1] = nodo.claves[i]
                nodo.libros[i + 1] = nodo.libros[i]
                i -= 1
            nodo.claves[i + 1] = codigo
            nodo.libros[i + 1] = libro
        else:
            while i >= 0 and codigo < nodo.claves[i]:
                i -= 1
            i += 1
            if len(nodo.hijos[i].claves) == 2 * self.t - 1:
                self._split(nodo, i, nodo.hijos[i])
                if codigo > nodo.claves[i]:
                    i += 1
            self._ins_no_lleno(nodo.hijos[i], codigo, libro)

    # ------------------------------------------------------------------ #
    # Eliminacion (3 casos)
    # ------------------------------------------------------------------ #
    def eliminar(self, codigo: str):
        if self.buscar(codigo) is None:
            raise KeyError(f"Codigo no encontrado: {codigo}")
        self._eliminar(self.raiz, codigo)
        if len(self.raiz.claves) == 0 and not self.raiz.es_hoja:
            self.raiz = self.raiz.hijos[0]  # la raiz se vacio tras fusion
        self._n -= 1

    def _eliminar(self, nodo: NodoB, codigo: str):
        t = self.t
        i = 0
        while i < len(nodo.claves) and codigo > nodo.claves[i]:
            i += 1

        if i < len(nodo.claves) and nodo.claves[i] == codigo:
            if nodo.es_hoja:                      # Caso 1
                nodo.claves.pop(i)
                nodo.libros.pop(i)
            else:                                  # Caso 2
                self._eliminar_interno(nodo, i)
        else:
            if nodo.es_hoja:
                raise KeyError(f"Codigo no encontrado: {codigo}")
            # Caso 3: la clave esta en el subarbol hijos[i]
            en_ultimo = (i == len(nodo.claves))
            if len(nodo.hijos[i].claves) == t - 1:
                self._llenar(nodo, i)
            if en_ultimo and i > len(nodo.claves):
                self._eliminar(nodo.hijos[i - 1], codigo)
            else:
                self._eliminar(nodo.hijos[i], codigo)

    def _eliminar_interno(self, nodo: NodoB, i: int):
        t = self.t
        codigo = nodo.claves[i]
        if len(nodo.hijos[i].claves) >= t:              # Caso 2a: predecesor
            pred, lib = self._obtener_predecesor(nodo, i)
            nodo.claves[i], nodo.libros[i] = pred, lib
            self._eliminar(nodo.hijos[i], pred)
        elif len(nodo.hijos[i + 1].claves) >= t:        # Caso 2b: sucesor
            suc, lib = self._obtener_sucesor(nodo, i)
            nodo.claves[i], nodo.libros[i] = suc, lib
            self._eliminar(nodo.hijos[i + 1], suc)
        else:                                            # Caso 2c: fusion
            self._fusionar(nodo, i)
            self._eliminar(nodo.hijos[i], codigo)

    def _obtener_predecesor(self, nodo: NodoB, i: int):
        cur = nodo.hijos[i]
        while not cur.es_hoja:
            cur = cur.hijos[-1]
        return cur.claves[-1], cur.libros[-1]

    def _obtener_sucesor(self, nodo: NodoB, i: int):
        cur = nodo.hijos[i + 1]
        while not cur.es_hoja:
            cur = cur.hijos[0]
        return cur.claves[0], cur.libros[0]

    def _fusionar(self, nodo: NodoB, i: int):
        """Fusiona hijos[i] y hijos[i+1] con la clave nodo.claves[i] en medio."""
        hijo, hermano = nodo.hijos[i], nodo.hijos[i + 1]
        hijo.claves.append(nodo.claves[i])
        hijo.libros.append(nodo.libros[i])
        hijo.claves.extend(hermano.claves)
        hijo.libros.extend(hermano.libros)
        if not hijo.es_hoja:
            hijo.hijos.extend(hermano.hijos)
        nodo.claves.pop(i)
        nodo.libros.pop(i)
        nodo.hijos.pop(i + 1)

    def _llenar(self, nodo: NodoB, i: int):
        """Asegura que hijos[i] tenga >= t claves antes de descender
        (borrow del hermano izquierdo o derecho, o fusion)."""
        t = self.t
        if i != 0 and len(nodo.hijos[i - 1].claves) >= t:
            self._prestar_de_anterior(nodo, i)
        elif i != len(nodo.claves) and len(nodo.hijos[i + 1].claves) >= t:
            self._prestar_de_siguiente(nodo, i)
        else:
            if i != len(nodo.claves):
                self._fusionar(nodo, i)
            else:
                self._fusionar(nodo, i - 1)

    def _prestar_de_anterior(self, nodo: NodoB, i: int):
        hijo, hermano = nodo.hijos[i], nodo.hijos[i - 1]
        hijo.claves.insert(0, nodo.claves[i - 1])
        hijo.libros.insert(0, nodo.libros[i - 1])
        if not hijo.es_hoja:
            hijo.hijos.insert(0, hermano.hijos.pop())
        nodo.claves[i - 1] = hermano.claves.pop()
        nodo.libros[i - 1] = hermano.libros.pop()

    def _prestar_de_siguiente(self, nodo: NodoB, i: int):
        hijo, hermano = nodo.hijos[i], nodo.hijos[i + 1]
        hijo.claves.append(nodo.claves[i])
        hijo.libros.append(nodo.libros[i])
        if not hijo.es_hoja:
            hijo.hijos.append(hermano.hijos.pop(0))
        nodo.claves[i] = hermano.claves.pop(0)
        nodo.libros[i] = hermano.libros.pop(0)

    # ------------------------------------------------------------------ #
    # Utilidades de verificacion (usadas por la prueba de estres)
    # ------------------------------------------------------------------ #
    def in_order(self) -> list[str]:
        """Retorna la lista de claves en orden ascendente."""
        salida: list[str] = []

        def _rec(nodo: NodoB):
            for i in range(len(nodo.claves)):
                if not nodo.es_hoja:
                    _rec(nodo.hijos[i])
                salida.append(nodo.claves[i])
            if not nodo.es_hoja:
                _rec(nodo.hijos[-1])

        _rec(self.raiz)
        return salida

    def altura(self) -> int:
        """Altura del arbol (0 si solo tiene la raiz como hoja)."""
        h = 0
        nodo = self.raiz
        while not nodo.es_hoja:
            h += 1
            nodo = nodo.hijos[0]
        return h

    def verificar_propiedades(self) -> bool:
        """Verifica: orden interno de cada nodo, rango de claves por nodo
        (salvo la raiz) y que todas las hojas esten al mismo nivel."""
        t = self.t
        profundidades_hoja = []

        def _rec(nodo: NodoB, es_raiz: bool, profundidad: int):
            # Orden interno
            if nodo.claves != sorted(nodo.claves):
                return False
            # Cantidad de claves
            maximo = 2 * t - 1
            minimo = 1 if es_raiz else t - 1
            if not (minimo <= len(nodo.claves) <= maximo) and not (es_raiz and len(nodo.claves) == 0):
                return False
            # Cantidad de hijos coherente
            if not nodo.es_hoja and len(nodo.hijos) != len(nodo.claves) + 1:
                return False
            if nodo.es_hoja:
                profundidades_hoja.append(profundidad)
                return True
            return all(_rec(h, False, profundidad + 1) for h in nodo.hijos)

        ok = _rec(self.raiz, True, 0)
        return ok and len(set(profundidades_hoja)) == 1


# ---------------------------------------------------------------------------
# Simulacion del catalogo de la Biblioteca Central UNA-PUNO
# ---------------------------------------------------------------------------
def generar_codigos(cantidad: int, semilla: int = 42) -> list[str]:
    rnd = random.Random(semilla)
    codigos = sorted(set(
        f"{rnd.randint(0, 999):03d}.{rnd.randint(0, 999):03d} "
        f"{chr(65 + rnd.randint(0, 25))}{rnd.randint(1, 99)}"
        for _ in range(int(cantidad * 1.0625))
    ))[:cantidad]
    return codigos


def simular_biblioteca(n_libros: int = 80_000, t: int = 50, semilla: int = 42):
    rnd = random.Random(semilla)
    biblioteca = ArbolBBiblioteca(t=t)

    codigos = generar_codigos(n_libros, semilla)
    codigos_barajados = codigos[:]
    rnd.shuffle(codigos_barajados)

    print(f"Indexando {len(codigos)} volumenes (t={t})...")
    for i, cod in enumerate(codigos_barajados):
        libro = Libro(codigo_topo=cod, titulo=f"Obra {i}", autor=f"Autor {i % 500}")
        biblioteca.insertar(cod, libro)
    print(f"Catalogo indexado. Altura del arbol: {biblioteca.altura()}")

    # Prestamos: eliminacion temporal
    prestamos = rnd.sample(codigos, 500)
    for cod in prestamos:
        biblioteca.eliminar(cod)
    print(f"{len(prestamos)} prestamos procesados (libros retirados del indice).")

    # Devoluciones: reinsercion de 300 de los 500 prestados
    devoluciones = prestamos[:300]
    for cod in devoluciones:
        libro = Libro(codigo_topo=cod, titulo="(reinsertado)", autor="(reinsertado)")
        biblioteca.insertar(cod, libro)
    print(f"{len(devoluciones)} devoluciones procesadas (libros reinsertados).")

    # Verificacion final: los codigos nunca devueltos (200) no deben encontrarse;
    # el resto (79,800) si deben encontrarse.
    prestados_no_devueltos = set(prestamos[300:])
    esperados = [c for c in codigos if c not in prestados_no_devueltos]
    encontrados = sum(1 for c in esperados if biblioteca.buscar(c) is not None)
    print(f"Libros indexados verificables tras la simulacion: "
          f"{encontrados}/{len(esperados)}")
    print(f"Propiedades del arbol validas: {biblioteca.verificar_propiedades()}")

    return biblioteca, codigos


def prueba_de_estres(biblioteca: ArbolBBiblioteca, codigos: list[str],
                      n_operaciones: int = 10_000, semilla: int = 7):
    """10,000 eliminaciones aleatorias seguidas de 10,000 inserciones
    aleatorias; verifica integridad con un recorrido in-order."""
    rnd = random.Random(semilla)
    presentes = [c for c in codigos if biblioteca.buscar(c) is not None]
    conjunto_esperado = set(presentes)  # lo que debe seguir estando tras la prueba
    a_eliminar = rnd.sample(presentes, min(n_operaciones, len(presentes)))

    for cod in a_eliminar:
        biblioteca.eliminar(cod)

    for i, cod in enumerate(a_eliminar):
        libro = Libro(codigo_topo=cod, titulo=f"Reinsertado {i}", autor="Estres")
        biblioteca.insertar(cod, libro)

    orden = biblioteca.in_order()
    ordenado_correctamente = orden == sorted(orden)
    sin_perdida = set(orden) == conjunto_esperado
    propiedades_ok = biblioteca.verificar_propiedades()

    print("\n=== PRUEBA DE ESTRES ===")
    print(f"Operaciones realizadas: {len(a_eliminar)} eliminaciones + "
          f"{len(a_eliminar)} inserciones")
    print(f"Orden preservado (in-order): {ordenado_correctamente}")
    print(f"Catalogo sin perdida de registros: {sin_perdida}")
    print(f"Propiedades del arbol B validas: {propiedades_ok}")
                                       

if __name__ == "__main__":
    print("=== PRUEBA ARBOL B - BIBLIOTECA CENTRAL UNA-PUNO ===\n")
    biblioteca, codigos = simular_biblioteca(n_libros=80_000, t=50)
    prueba_de_estres(biblioteca, codigos, n_operaciones=10_000)

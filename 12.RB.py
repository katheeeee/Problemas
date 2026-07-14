from enum import Enum, auto


class Color(Enum):
    ROJO = auto()
    NEGRO = auto()


class NodoRBT:
    def __init__(self, votante=None, color=Color.ROJO):
        self.votante = votante
        self.izquierdo = None
        self.derecho = None
        self.padre = None
        self.color = color

    def __repr__(self):
        c = 'R' if self.color == Color.ROJO else 'N'
        dni = self.votante[0] if self.votante else 'NIL'
        return f'[{dni}:{c}]'


NIL = NodoRBT(votante=None, color=Color.NEGRO)
NIL.izquierdo = NIL.derecho = NIL.padre = NIL


def verificar_p2(raiz):
    return raiz is NIL or raiz.color == Color.NEGRO


def verificar_p4(nodo):
    if nodo is NIL:
        return True
    if nodo.color == Color.ROJO:
        if nodo.izquierdo.color == Color.ROJO:
            return False
        if nodo.derecho.color == Color.ROJO:
            return False
    return verificar_p4(nodo.izquierdo) and verificar_p4(nodo.derecho)


def altura_negra(nodo):
    if nodo is NIL:
        return 0
    bh_izq = altura_negra(nodo.izquierdo)
    bh_der = altura_negra(nodo.derecho)
    if bh_izq == -1 or bh_der == -1 or bh_izq != bh_der:
        return -1
    return bh_izq + (1 if nodo.color == Color.NEGRO else 0)


def verificar_todas(raiz):
    ok_p2 = verificar_p2(raiz)
    ok_p4 = verificar_p4(raiz)
    ok_p5 = altura_negra(raiz) != -1
    return ok_p2 and ok_p4 and ok_p5


class ArbolElectoral:
    def __init__(self):
        self.raiz = NIL

    def _rot_izq(self, x):
        y = x.derecho
        x.derecho = y.izquierdo
        if y.izquierdo is not NIL:
            y.izquierdo.padre = x
        y.padre = x.padre
        if x.padre is NIL:
            self.raiz = y
        elif x == x.padre.izquierdo:
            x.padre.izquierdo = y
        else:
            x.padre.derecho = y
        y.izquierdo = x
        x.padre = y

    def _rot_der(self, y):
        x = y.izquierdo
        y.izquierdo = x.derecho
        if x.derecho is not NIL:
            x.derecho.padre = y
        x.padre = y.padre
        if y.padre is NIL:
            self.raiz = x
        elif y == y.padre.derecho:
            y.padre.derecho = x
        else:
            y.padre.izquierdo = x
        x.derecho = y
        y.padre = x

    def insertar(self, votante):
        z = NodoRBT(votante=votante)
        z.izquierdo = z.derecho = z.padre = NIL
        y, x = NIL, self.raiz
        while x is not NIL:
            y = x
            x = x.izquierdo if votante[0] < x.votante[0] else x.derecho
        z.padre = y
        if y is NIL:
            self.raiz = z
        elif votante[0] < y.votante[0]:
            y.izquierdo = z
        else:
            y.derecho = z
        self._fix_insertar(z)

    def _fix_insertar(self, z):
        while z.padre.color == Color.ROJO:
            if z.padre == z.padre.padre.izquierdo:
                tio = z.padre.padre.derecho
                if tio.color == Color.ROJO:
                    z.padre.color = Color.NEGRO
                    tio.color = Color.NEGRO
                    z.padre.padre.color = Color.ROJO
                    z = z.padre.padre
                else:
                    if z == z.padre.derecho:
                        z = z.padre
                        self._rot_izq(z)
                    z.padre.color = Color.NEGRO
                    z.padre.padre.color = Color.ROJO
                    self._rot_der(z.padre.padre)
            else:
                tio = z.padre.padre.izquierdo
                if tio.color == Color.ROJO:
                    z.padre.color = Color.NEGRO
                    tio.color = Color.NEGRO
                    z.padre.padre.color = Color.ROJO
                    z = z.padre.padre
                else:
                    if z == z.padre.izquierdo:
                        z = z.padre
                        self._rot_der(z)
                    z.padre.color = Color.NEGRO
                    z.padre.padre.color = Color.ROJO
                    self._rot_izq(z.padre.padre)
        self.raiz.color = Color.NEGRO

    def buscar(self, dni):
        x = self.raiz
        while x is not NIL and x.votante[0] != dni:
            x = x.izquierdo if dni < x.votante[0] else x.derecho
        return x if x is not NIL else None

    def _transplant(self, u, v):
        if u.padre is NIL:
            self.raiz = v
        elif u == u.padre.izquierdo:
            u.padre.izquierdo = v
        else:
            u.padre.derecho = v
        v.padre = u.padre

    def _minimo(self, n):
        while n.izquierdo is not NIL:
            n = n.izquierdo
        return n

    def eliminar(self, dni):
        z = self.buscar(dni)
        if z is None:
            raise KeyError(f'DNI no encontrado: {dni}')
        y = z
        y_color_original = y.color
        if z.izquierdo is NIL:
            x = z.derecho
            self._transplant(z, z.derecho)
        elif z.derecho is NIL:
            x = z.izquierdo
            self._transplant(z, z.izquierdo)
        else:
            y = self._minimo(z.derecho)
            y_color_original = y.color
            x = y.derecho
            if y.padre == z:
                x.padre = y
            else:
                self._transplant(y, y.derecho)
                y.derecho = z.derecho
                y.derecho.padre = y
            self._transplant(z, y)
            y.izquierdo = z.izquierdo
            y.izquierdo.padre = y
            y.color = z.color
        if y_color_original == Color.NEGRO:
            self._fix_eliminar(x)

    def _fix_eliminar(self, x):
        while x != self.raiz and x.color == Color.NEGRO:
            if x == x.padre.izquierdo:
                w = x.padre.derecho
                if w.color == Color.ROJO:
                    w.color = Color.NEGRO
                    x.padre.color = Color.ROJO
                    self._rot_izq(x.padre)
                    w = x.padre.derecho
                if w.izquierdo.color == Color.NEGRO and w.derecho.color == Color.NEGRO:
                    w.color = Color.ROJO
                    x = x.padre
                else:
                    if w.derecho.color == Color.NEGRO:
                        w.izquierdo.color = Color.NEGRO
                        w.color = Color.ROJO
                        self._rot_der(w)
                        w = x.padre.derecho
                    w.color = x.padre.color
                    x.padre.color = Color.NEGRO
                    w.derecho.color = Color.NEGRO
                    self._rot_izq(x.padre)
                    x = self.raiz
            else:
                w = x.padre.izquierdo
                if w.color == Color.ROJO:
                    w.color = Color.NEGRO
                    x.padre.color = Color.ROJO
                    self._rot_der(x.padre)
                    w = x.padre.izquierdo
                if w.derecho.color == Color.NEGRO and w.izquierdo.color == Color.NEGRO:
                    w.color = Color.ROJO
                    x = x.padre
                else:
                    if w.izquierdo.color == Color.NEGRO:
                        w.derecho.color = Color.NEGRO
                        w.color = Color.ROJO
                        self._rot_izq(w)
                        w = x.padre.izquierdo
                    w.color = x.padre.color
                    x.padre.color = Color.NEGRO
                    w.izquierdo.color = Color.NEGRO
                    self._rot_der(x.padre)
                    x = self.raiz
        x.color = Color.NEGRO


padron = ArbolElectoral()
votantes = [
    ('70512340', 'Mamani Quispe, J.', 'Ing. Sistemas', True),
    ('70512100', 'Huanca Apaza, M.', 'Ing. Civil', True),
    ('70512700', 'Condori Flores, P.', 'Medicina', True),
    ('70512050', 'Ticona Lupaca, R.', 'Contabilidad', False),
    ('70512900', 'Pari Choque, L.', 'Agronomia', True),
]

for v in votantes:
    padron.insertar(v)

assert verificar_todas(padron.raiz), 'Propiedades RBT violadas tras inserción'
print('Inserción: 5 propiedades VERIFICADAS ✓')

padron.eliminar('70512050')
assert verificar_todas(padron.raiz), 'Propiedades RBT violadas tras eliminación'
print('Eliminación: 5 propiedades VERIFICADAS ✓')

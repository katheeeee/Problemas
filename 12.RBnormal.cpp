// ── rbt.cpp — Arbol Rojo-Negro (RBT) con inserción y eliminación ───────
// Archivo único y autocontenido: compilar con
//   g++ -std=c++17 -O2 -Wall -o rbt rbt.cpp && ./rbt
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

enum class Color { ROJO, NEGRO };

struct Votante {
    std::string dni, nombre, facultad;
    bool habilitado;
};

struct NodoRBT {
    Votante votante;
    Color color = Color::ROJO;
    NodoRBT* izquierdo = nullptr;
    NodoRBT* derecho   = nullptr;
    NodoRBT* padre     = nullptr;
};

class ArbolElectoral {
    NodoRBT* NIL;
    NodoRBT* raiz;
    std::vector<NodoRBT*> todos;   // para liberar memoria de los nodos vivos

    // ── Rotaciones ────────────────────────────────────────────────────
    void rotIzq(NodoRBT* x) {
        NodoRBT* y = x->derecho;
        x->derecho = y->izquierdo;
        if (y->izquierdo != NIL) y->izquierdo->padre = x;
        y->padre = x->padre;
        if (x->padre == NIL) raiz = y;
        else if (x == x->padre->izquierdo) x->padre->izquierdo = y;
        else x->padre->derecho = y;
        y->izquierdo = x;
        x->padre = y;
    }

    void rotDer(NodoRBT* y) {
        NodoRBT* x = y->izquierdo;
        y->izquierdo = x->derecho;
        if (x->derecho != NIL) x->derecho->padre = y;
        x->padre = y->padre;
        if (y->padre == NIL) raiz = x;
        else if (y == y->padre->derecho) y->padre->derecho = x;
        else y->padre->izquierdo = x;
        x->derecho = y;
        y->padre = x;
    }

    // ── Fix-up de inserción ──────────────────────────────────────────
    void fixInsertar(NodoRBT* z) {
        while (z->padre->color == Color::ROJO) {
            if (z->padre == z->padre->padre->izquierdo) {
                NodoRBT* tio = z->padre->padre->derecho;
                if (tio->color == Color::ROJO) {
                    z->padre->color = tio->color = Color::NEGRO;
                    z->padre->padre->color = Color::ROJO;
                    z = z->padre->padre;
                } else {
                    if (z == z->padre->derecho) {
                        z = z->padre;
                        rotIzq(z);
                    }
                    z->padre->color = Color::NEGRO;
                    z->padre->padre->color = Color::ROJO;
                    rotDer(z->padre->padre);
                }
            } else { // simétrico
                NodoRBT* tio = z->padre->padre->izquierdo;
                if (tio->color == Color::ROJO) {
                    z->padre->color = tio->color = Color::NEGRO;
                    z->padre->padre->color = Color::ROJO;
                    z = z->padre->padre;
                } else {
                    if (z == z->padre->izquierdo) {
                        z = z->padre;
                        rotDer(z);
                    }
                    z->padre->color = Color::NEGRO;
                    z->padre->padre->color = Color::ROJO;
                    rotIzq(z->padre->padre);
                }
            }
        }
        raiz->color = Color::NEGRO;
    }

    // ── Auxiliares de eliminación ────────────────────────────────────
    NodoRBT* minimo(NodoRBT* x) const {
        while (x->izquierdo != NIL) x = x->izquierdo;
        return x;
    }

    void trasplantar(NodoRBT* u, NodoRBT* v) {
        if (u->padre == NIL) raiz = v;
        else if (u == u->padre->izquierdo) u->padre->izquierdo = v;
        else u->padre->derecho = v;
        v->padre = u->padre;
    }

    NodoRBT* buscarNodo(const std::string& dni) const {
        NodoRBT* x = raiz;
        while (x != NIL && x->votante.dni != dni)
            x = (dni < x->votante.dni) ? x->izquierdo : x->derecho;
        return x;
    }

    // ── Fix-up de eliminación (6 casos, con centinela NIL) ────────────
    void fixEliminar(NodoRBT* x) {
        while (x != raiz && x->color == Color::NEGRO) {
            if (x == x->padre->izquierdo) {
                NodoRBT* w = x->padre->derecho;
                if (w->color == Color::ROJO) {
                    w->color = Color::NEGRO;
                    x->padre->color = Color::ROJO;
                    rotIzq(x->padre);
                    w = x->padre->derecho;
                }
                if (w->izquierdo->color == Color::NEGRO &&
                    w->derecho->color == Color::NEGRO) {
                    w->color = Color::ROJO;
                    x = x->padre;
                } else {
                    if (w->derecho->color == Color::NEGRO) {
                        w->izquierdo->color = Color::NEGRO;
                        w->color = Color::ROJO;
                        rotDer(w);
                        w = x->padre->derecho;
                    }
                    w->color = x->padre->color;
                    x->padre->color = Color::NEGRO;
                    w->derecho->color = Color::NEGRO;
                    rotIzq(x->padre);
                    x = raiz;
                }
            } else { // simétrico
                NodoRBT* w = x->padre->izquierdo;
                if (w->color == Color::ROJO) {
                    w->color = Color::NEGRO;
                    x->padre->color = Color::ROJO;
                    rotDer(x->padre);
                    w = x->padre->izquierdo;
                }
                if (w->derecho->color == Color::NEGRO &&
                    w->izquierdo->color == Color::NEGRO) {
                    w->color = Color::ROJO;
                    x = x->padre;
                } else {
                    if (w->izquierdo->color == Color::NEGRO) {
                        w->derecho->color = Color::NEGRO;
                        w->color = Color::ROJO;
                        rotIzq(w);
                        w = x->padre->izquierdo;
                    }
                    w->color = x->padre->color;
                    x->padre->color = Color::NEGRO;
                    w->izquierdo->color = Color::NEGRO;
                    rotDer(x->padre);
                    x = raiz;
                }
            }
        }
        x->color = Color::NEGRO;
    }

public:
    ArbolElectoral() {
        NIL = new NodoRBT();
        NIL->color = Color::NEGRO;
        NIL->izquierdo = NIL->derecho = NIL->padre = NIL;
        raiz = NIL;
        todos.push_back(NIL);
    }

    ~ArbolElectoral() {
        for (auto* n : todos) delete n;
    }

    // ── Inserción ──────────────────────────────────────────────────
    void insertar(Votante v) {
        NodoRBT* z = new NodoRBT();
        z->votante = v;
        z->izquierdo = z->derecho = z->padre = NIL;
        todos.push_back(z);

        NodoRBT *y = NIL, *x = raiz;
        while (x != NIL) {
            y = x;
            x = (v.dni < x->votante.dni) ? x->izquierdo : x->derecho;
        }
        z->padre = y;
        if (y == NIL) raiz = z;
        else if (v.dni < y->votante.dni) y->izquierdo = z;
        else y->derecho = z;

        fixInsertar(z);
    }

    // ── Eliminación (baja de habilitados) ─────────────────────────
    bool eliminar(const std::string& dni) {
        NodoRBT* z = buscarNodo(dni);
        if (z == NIL) return false;

        NodoRBT* y = z;
        NodoRBT* x;
        Color colorOriginalY = y->color;

        if (z->izquierdo == NIL) {
            x = z->derecho;
            trasplantar(z, z->derecho);
        } else if (z->derecho == NIL) {
            x = z->izquierdo;
            trasplantar(z, z->izquierdo);
        } else {
            y = minimo(z->derecho);
            colorOriginalY = y->color;
            x = y->derecho;
            if (y->padre == z) {
                x->padre = y;
            } else {
                trasplantar(y, y->derecho);
                y->derecho = z->derecho;
                y->derecho->padre = y;
            }
            trasplantar(z, y);
            y->izquierdo = z->izquierdo;
            y->izquierdo->padre = y;
            y->color = z->color;
        }

        if (colorOriginalY == Color::NEGRO) fixEliminar(x);

        // liberar memoria del nodo eliminado (sin fugas)
        todos.erase(std::remove(todos.begin(), todos.end(), z), todos.end());
        delete z;
        return true;
    }

    // ── Búsqueda ───────────────────────────────────────────────────
    NodoRBT* buscar(const std::string& dni) const {
        NodoRBT* x = buscarNodo(dni);
        return x == NIL ? nullptr : x;
    }

    // ── Verificadores de propiedades RBT ───────────────────────────
    bool esRBTValido() const {
        return raiz->color == Color::NEGRO && p4(raiz) && p5(raiz) != -1;
    }

    int altura(NodoRBT* n) const {
        if (n == NIL) return 0;
        return 1 + std::max(altura(n->izquierdo), altura(n->derecho));
    }
    int altura() const { return altura(raiz); }

private:
    bool p4(NodoRBT* n) const { // no dos rojos consecutivos
        if (n == NIL) return true;
        if (n->color == Color::ROJO &&
            (n->izquierdo->color == Color::ROJO || n->derecho->color == Color::ROJO))
            return false;
        return p4(n->izquierdo) && p4(n->derecho);
    }

    int p5(NodoRBT* n) const { // altura negra uniforme
        if (n == NIL) return 0;
        int bl = p5(n->izquierdo), br = p5(n->derecho);
        if (bl == -1 || br == -1 || bl != br) return -1;
        return bl + (n->color == Color::NEGRO ? 1 : 0);
    }
};

int main() {
    ArbolElectoral padron;

    padron.insertar({"70512340","Mamani Quispe, J.","Ing. Sistemas",true});
    padron.insertar({"70512100","Huanca Apaza, M.", "Ing. Civil", true});
    padron.insertar({"70512700","Condori Flores, P.","Medicina", true});
    padron.insertar({"70512050","Ticona Lupaca, R.", "Contabilidad",false});
    padron.insertar({"70512900","Pari Choque, L.", "Agronomia", true});

    std::cout << "RBT valido tras insercion: " << padron.esRBTValido() << '\n';

    auto* v = padron.buscar("70512700");
    std::cout << "Buscar 70512700: " << (v ? v->votante.nombre : "no encontrado") << '\n';

    // ── Prueba de eliminación (baja por deuda académica / retiro) ──
    bool ok = padron.eliminar("70512050"); // Ticona Lupaca, R. (no habilitado)
    std::cout << "Eliminar 70512050: " << (ok ? "OK" : "no encontrado") << '\n';
    std::cout << "RBT valido tras eliminacion: " << padron.esRBTValido() << '\n';

    auto* v2 = padron.buscar("70512050");
    std::cout << "Buscar 70512050 tras baja: " << (v2 ? v2->votante.nombre : "no encontrado") << '\n';

    // eliminar el resto para comprobar estabilidad del árbol
    for (const std::string dni : {"70512340","70512100","70512700","70512900"}) {
        padron.eliminar(dni);
        std::cout << "Tras eliminar " << dni << " -> RBT valido: " << padron.esRBTValido() << '\n';
    }

    return 0;
}

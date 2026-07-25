
#include "rbt.hpp"
#include <algorithm>

NodoRBT::NodoRBT()
    : votante(), color(Color::ROJO), izquierdo(nullptr), derecho(nullptr), padre(nullptr) {}

ArbolElectoral::ArbolElectoral() {
    NIL = new NodoRBT();
    NIL->color = Color::NEGRO;
    NIL->izquierdo = NIL;
    NIL->derecho = NIL;
    NIL->padre = NIL;
    raiz = NIL;
}

ArbolElectoral::~ArbolElectoral() {
    for (NodoRBT* n : todos) delete n;
    delete NIL;
}

void ArbolElectoral::rotIzq(NodoRBT* x) {
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

void ArbolElectoral::rotDer(NodoRBT* y) {
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

void ArbolElectoral::fixInsertar(NodoRBT* z) {
    while (z->padre->color == Color::ROJO) {
        if (z->padre == z->padre->padre->izquierdo) {
            NodoRBT* y = z->padre->padre->derecho;
            if (y->color == Color::ROJO) {
                z->padre->color = Color::NEGRO;
                y->color = Color::NEGRO;
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
        } else {
            NodoRBT* y = z->padre->padre->izquierdo;
            if (y->color == Color::ROJO) {
                z->padre->color = Color::NEGRO;
                y->color = Color::NEGRO;
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

void ArbolElectoral::insertar(Votante v) {
    NodoRBT* y = NIL;
    NodoRBT* x = raiz;
    while (x != NIL) {
        y = x;
        if (v.dni < x->votante.dni) x = x->izquierdo;
        else if (v.dni > x->votante.dni) x = x->derecho;
        else { x->votante = v; return; }
    }
    NodoRBT* z = new NodoRBT();
    z->votante = v;
    z->izquierdo = NIL;
    z->derecho = NIL;
    z->color = Color::ROJO;
    z->padre = y;
    todos.push_back(z);
    if (y == NIL) raiz = z;
    else if (z->votante.dni < y->votante.dni) y->izquierdo = z;
    else y->derecho = z;
    fixInsertar(z);
}

NodoRBT* ArbolElectoral::minimo(NodoRBT* x) const {
    while (x->izquierdo != NIL) x = x->izquierdo;
    return x;
}

void ArbolElectoral::trasplantar(NodoRBT* u, NodoRBT* v) {
    if (u->padre == NIL) raiz = v;
    else if (u == u->padre->izquierdo) u->padre->izquierdo = v;
    else u->padre->derecho = v;
    v->padre = u->padre;
}

NodoRBT* ArbolElectoral::buscarNodo(const std::string& dni) const {
    NodoRBT* x = raiz;
    while (x != NIL && x->votante.dni != dni) {
        if (dni < x->votante.dni) x = x->izquierdo;
        else x = x->derecho;
    }
    return x;
}

void ArbolElectoral::fixEliminar(NodoRBT* x) {
    while (x != raiz && x->color == Color::NEGRO) {
        if (x == x->padre->izquierdo) {
            NodoRBT* w = x->padre->derecho;
            if (w->color == Color::ROJO) {
                w->color = Color::NEGRO;
                x->padre->color = Color::ROJO;
                rotIzq(x->padre);
                w = x->padre->derecho;
            }
            if (w->izquierdo->color == Color::NEGRO && w->derecho->color == Color::NEGRO) {
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
        } else {
            NodoRBT* w = x->padre->izquierdo;
            if (w->color == Color::ROJO) {
                w->color = Color::NEGRO;
                x->padre->color = Color::ROJO;
                rotDer(x->padre);
                w = x->padre->izquierdo;
            }
            if (w->derecho->color == Color::NEGRO && w->izquierdo->color == Color::NEGRO) {
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

bool ArbolElectoral::eliminar(const std::string& dni) {
    NodoRBT* z = buscarNodo(dni);
    if (z == NIL) return false;
    NodoRBT* y = z;
    Color yColorOriginal = y->color;
    NodoRBT* x;
    if (z->izquierdo == NIL) {
        x = z->derecho;
        trasplantar(z, z->derecho);
    } else if (z->derecho == NIL) {
        x = z->izquierdo;
        trasplantar(z, z->izquierdo);
    } else {
        y = minimo(z->derecho);
        yColorOriginal = y->color;
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
    if (yColorOriginal == Color::NEGRO) fixEliminar(x);
    return true;
}

NodoRBT* ArbolElectoral::buscar(const std::string& dni) const {
    NodoRBT* n = buscarNodo(dni);
    return (n == NIL) ? nullptr : n;
}

bool ArbolElectoral::p4(NodoRBT* n) const {
    if (n == NIL) return true;
    if (n->color == Color::ROJO) {
        if (n->izquierdo->color == Color::ROJO || n->derecho->color == Color::ROJO) return false;
    }
    return p4(n->izquierdo) && p4(n->derecho);
}

int ArbolElectoral::p5(NodoRBT* n) const {
    if (n == NIL) return 1;
    int izq = p5(n->izquierdo);
    if (izq == -1) return -1;
    int der = p5(n->derecho);
    if (der == -1) return -1;
    if (izq != der) return -1;
    return izq + (n->color == Color::NEGRO ? 1 : 0);
}

bool ArbolElectoral::esRBTValido() const {
    if (raiz == NIL) return true;
    if (raiz->color != Color::NEGRO) return false;
    if (!p4(raiz)) return false;
    if (p5(raiz) == -1) return false;
    return true;
}

int ArbolElectoral::altura(NodoRBT* n) const {
    if (n == NIL) return 0;
    return 1 + std::max(altura(n->izquierdo), altura(n->derecho));
}

int ArbolElectoral::altura() const {
    return altura(raiz);
}

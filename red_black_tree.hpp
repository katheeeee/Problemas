#ifndef RED_BLACK_TREE_HPP
#define RED_BLACK_TREE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

struct Votante {
    std::string codigo;
    std::string nombre;
    std::string dni;

    Votante() : codigo(""), nombre(""), dni("") {}
    Votante(std::string c, std::string n, std::string d)
        : codigo(c), nombre(n), dni(d) {}
};

class RedBlackTree {
private:
    enum Color { ROJO, NEGRO };

    struct Nodo {
        int clave;
        Votante data;
        Color color;
        Nodo* izquierdo;
        Nodo* derecho;
        Nodo* padre;

        Nodo(int k, const Votante& v)
            : clave(k), data(v), color(ROJO),
              izquierdo(nullptr), derecho(nullptr), padre(nullptr) {}
    };

    Nodo* raiz;
    int num_nodos;
    int contador_rotaciones;
    Nodo* nulo;

    void inicializarNulo() {
        nulo = new Nodo(0, Votante());
        nulo->color = NEGRO;
        nulo->izquierdo = nullptr;
        nulo->derecho = nullptr;
        nulo->padre = nullptr;
    }

    void rotacionIzquierda(Nodo* x) {
        Nodo* y = x->derecho;
        x->derecho = y->izquierdo;
        if (y->izquierdo != nulo)
            y->izquierdo->padre = x;
        y->padre = x->padre;
        if (!x->padre)
            raiz = y;
        else if (x == x->padre->izquierdo)
            x->padre->izquierdo = y;
        else
            x->padre->derecho = y;
        y->izquierdo = x;
        x->padre = y;
        contador_rotaciones++;
    }

    void rotacionDerecha(Nodo* x) {
        Nodo* y = x->izquierdo;
        x->izquierdo = y->derecho;
        if (y->derecho != nulo)
            y->derecho->padre = x;
        y->padre = x->padre;
        if (!x->padre)
            raiz = y;
        else if (x == x->padre->derecho)
            x->padre->derecho = y;
        else
            x->padre->izquierdo = y;
        y->derecho = x;
        x->padre = y;
        contador_rotaciones++;
    }

    void insertarFijar(Nodo* z) {
        while (z->padre && z->padre->color == ROJO) {
            if (z->padre == z->padre->padre->izquierdo) {
                Nodo* tio = z->padre->padre->derecho;
                if (tio->color == ROJO) {
                    z->padre->color = NEGRO;
                    tio->color = NEGRO;
                    z->padre->padre->color = ROJO;
                    z = z->padre->padre;
                } else {
                    if (z == z->padre->derecho) {
                        z = z->padre;
                        rotacionIzquierda(z);
                    }
                    z->padre->color = NEGRO;
                    z->padre->padre->color = ROJO;
                    rotacionDerecha(z->padre->padre);
                }
            } else {
                Nodo* tio = z->padre->padre->izquierdo;
                if (tio->color == ROJO) {
                    z->padre->color = NEGRO;
                    tio->color = NEGRO;
                    z->padre->padre->color = ROJO;
                    z = z->padre->padre;
                } else {
                    if (z == z->padre->izquierdo) {
                        z = z->padre;
                        rotacionDerecha(z);
                    }
                    z->padre->color = NEGRO;
                    z->padre->padre->color = ROJO;
                    rotacionIzquierda(z->padre->padre);
                }
            }
        }
        raiz->color = NEGRO;
    }

    void transplantar(Nodo* u, Nodo* v) {
        if (!u->padre)
            raiz = v;
        else if (u == u->padre->izquierdo)
            u->padre->izquierdo = v;
        else
            u->padre->derecho = v;
        v->padre = u->padre;
    }

    void eliminarFijar(Nodo* x) {
        while (x != raiz && x->color == NEGRO) {
            if (x == x->padre->izquierdo) {
                Nodo* w = x->padre->derecho;
                if (w->color == ROJO) {
                    w->color = NEGRO;
                    x->padre->color = ROJO;
                    rotacionIzquierda(x->padre);
                    w = x->padre->derecho;
                }
                if (w->izquierdo->color == NEGRO && w->derecho->color == NEGRO) {
                    w->color = ROJO;
                    x = x->padre;
                } else {
                    if (w->derecho->color == NEGRO) {
                        w->izquierdo->color = NEGRO;
                        w->color = ROJO;
                        rotacionDerecha(w);
                        w = x->padre->derecho;
                    }
                    w->color = x->padre->color;
                    x->padre->color = NEGRO;
                    w->derecho->color = NEGRO;
                    rotacionIzquierda(x->padre);
                    x = raiz;
                }
            } else {
                Nodo* w = x->padre->izquierdo;
                if (w->color == ROJO) {
                    w->color = NEGRO;
                    x->padre->color = ROJO;
                    rotacionDerecha(x->padre);
                    w = x->padre->izquierdo;
                }
                if (w->derecho->color == NEGRO && w->izquierdo->color == NEGRO) {
                    w->color = ROJO;
                    x = x->padre;
                } else {
                    if (w->izquierdo->color == NEGRO) {
                        w->derecho->color = NEGRO;
                        w->color = ROJO;
                        rotacionIzquierda(w);
                        w = x->padre->izquierdo;
                    }
                    w->color = x->padre->color;
                    x->padre->color = NEGRO;
                    w->izquierdo->color = NEGRO;
                    rotacionDerecha(x->padre);
                    x = raiz;
                }
            }
        }
        x->color = NEGRO;
    }

    Nodo* minimo(Nodo* nodo) {
        while (nodo->izquierdo != nulo)
            nodo = nodo->izquierdo;
        return nodo;
    }

    Nodo* buscarNodo(Nodo* nodo, int clave) {
        if (!nodo || nodo == nulo) return nullptr;
        if (clave == nodo->clave) return nodo;
        if (clave < nodo->clave)
            return buscarNodo(nodo->izquierdo, clave);
        return buscarNodo(nodo->derecho, clave);
    }

    void inorden(Nodo* nodo, std::vector<std::pair<int, Votante>>& resultado) {
        if (!nodo || nodo == nulo) return;
        inorden(nodo->izquierdo, resultado);
        resultado.push_back({nodo->clave, nodo->data});
        inorden(nodo->derecho, resultado);
    }

    int calcularAltura(Nodo* nodo) {
        if (!nodo || nodo == nulo) return 0;
        return 1 + std::max(calcularAltura(nodo->izquierdo),
                            calcularAltura(nodo->derecho));
    }

    void destruir(Nodo* nodo) {
        if (!nodo || nodo == nulo) return;
        destruir(nodo->izquierdo);
        destruir(nodo->derecho);
        delete nodo;
    }

public:
    RedBlackTree() : raiz(nullptr), num_nodos(0), contador_rotaciones(0) {
        inicializarNulo();
        raiz = nulo;
    }

    ~RedBlackTree() {
        destruir(raiz);
        delete nulo;
    }

    void insertar(int clave, const Votante& data) {
        if (buscarNodo(raiz, clave)) {
            Nodo* existente = buscarNodo(raiz, clave);
            existente->data = data;
            return;
        }

        Nodo* z = new Nodo(clave, data);
        z->izquierdo = nulo;
        z->derecho = nulo;

        Nodo* y = nullptr;
        Nodo* x = raiz;

        while (x != nulo) {
            y = x;
            if (z->clave < x->clave)
                x = x->izquierdo;
            else
                x = x->derecho;
        }

        z->padre = y;
        if (!y)
            raiz = z;
        else if (z->clave < y->clave)
            y->izquierdo = z;
        else
            y->derecho = z;

        num_nodos++;
        insertarFijar(z);
    }

    bool eliminar(int clave) {
        Nodo* z = buscarNodo(raiz, clave);
        if (!z) return false;

        Nodo* y = z;
        Nodo* x;
        Color y_color_original = y->color;

        if (z->izquierdo == nulo) {
            x = z->derecho;
            transplantar(z, z->derecho);
        } else if (z->derecho == nulo) {
            x = z->izquierdo;
            transplantar(z, z->izquierdo);
        } else {
            y = minimo(z->derecho);
            y_color_original = y->color;
            x = y->derecho;
            if (y->padre == z) {
                x->padre = y;
            } else {
                transplantar(y, y->derecho);
                y->derecho = z->derecho;
                y->derecho->padre = y;
            }
            transplantar(z, y);
            y->izquierdo = z->izquierdo;
            y->izquierdo->padre = y;
            y->color = z->color;
        }

        delete z;
        num_nodos--;

        if (y_color_original == NEGRO)
            eliminarFijar(x);

        return true;
    }

    Votante* buscar(int clave) {
        Nodo* resultado = buscarNodo(raiz, clave);
        return resultado ? &(resultado->data) : nullptr;
    }

    std::vector<std::pair<int, Votante>> inorden() {
        std::vector<std::pair<int, Votante>> resultado;
        inorden(raiz, resultado);
        return resultado;
    }

    int obtenerAltura() { return calcularAltura(raiz); }
    int obtenerNumNodos() { return num_nodos; }
    int obtenerContadorRotaciones() { return contador_rotaciones; }
};

#endif

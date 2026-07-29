#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

struct Postulante {
    std::string codigo;
    std::string nombre;
    std::string region;
    double puntaje;

    Postulante() : codigo(""), nombre(""), region(""), puntaje(0.0) {}
    Postulante(std::string c, std::string n, std::string r, double p)
        : codigo(c), nombre(n), region(r), puntaje(p) {}
};

class AVLTree {
private:
    struct Nodo {
        Postulante data;
        int altura;
        Nodo* izquierdo;
        Nodo* derecho;

        Nodo(const Postulante& d)
            : data(d), altura(1), izquierdo(nullptr), derecho(nullptr) {}
    };

    Nodo* raiz;
    int num_nodos;
    int contador_rotaciones;

    static bool cmp(const Postulante& a, const Postulante& b) {
        if (std::abs(a.puntaje - b.puntaje) > 1e-9)
            return a.puntaje < b.puntaje;
        return a.codigo < b.codigo;
    }

    int obtenerAltura(Nodo* n) {
        return n ? n->altura : 0;
    }

    int obtenerBalance(Nodo* n) {
        return n ? obtenerAltura(n->izquierdo) - obtenerAltura(n->derecho) : 0;
    }

    void actualizarAltura(Nodo* n) {
        if (n) {
            n->altura = 1 + std::max(obtenerAltura(n->izquierdo),
                                      obtenerAltura(n->derecho));
        }
    }

    Nodo* rotacionDerecha(Nodo* y) {
        Nodo* x = y->izquierdo;
        Nodo* T2 = x->derecho;
        x->derecho = y;
        y->izquierdo = T2;
        actualizarAltura(y);
        actualizarAltura(x);
        contador_rotaciones++;
        return x;
    }

    Nodo* rotacionIzquierda(Nodo* x) {
        Nodo* y = x->derecho;
        Nodo* T2 = y->izquierdo;
        y->izquierdo = x;
        x->derecho = T2;
        actualizarAltura(x);
        actualizarAltura(y);
        contador_rotaciones++;
        return y;
    }

    Nodo* insertar(Nodo* nodo, const Postulante& data) {
        if (!nodo) {
            num_nodos++;
            return new Nodo(data);
        }

        if (cmp(data, nodo->data))
            nodo->izquierdo = insertar(nodo->izquierdo, data);
        else if (cmp(nodo->data, data))
            nodo->derecho = insertar(nodo->derecho, data);
        else
            return nodo;

        actualizarAltura(nodo);

        int balance = obtenerBalance(nodo);

        if (balance > 1 && cmp(data, nodo->izquierdo->data))
            return rotacionDerecha(nodo);

        if (balance < -1 && cmp(nodo->derecho->data, data))
            return rotacionIzquierda(nodo);

        if (balance > 1 && cmp(nodo->izquierdo->data, data)) {
            nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
            return rotacionDerecha(nodo);
        }

        if (balance < -1 && cmp(data, nodo->derecho->data)) {
            nodo->derecho = rotacionDerecha(nodo->derecho);
            return rotacionIzquierda(nodo);
        }

        return nodo;
    }

    Nodo* minValor(Nodo* nodo) {
        Nodo* actual = nodo;
        while (actual && actual->izquierdo)
            actual = actual->izquierdo;
        return actual;
    }

    Nodo* maxValor(Nodo* nodo) {
        Nodo* actual = nodo;
        while (actual && actual->derecho)
            actual = actual->derecho;
        return actual;
    }

    Nodo* eliminar(Nodo* nodo, const Postulante& data) {
        if (!nodo)
            return nullptr;

        if (cmp(data, nodo->data))
            nodo->izquierdo = eliminar(nodo->izquierdo, data);
        else if (cmp(nodo->data, data))
            nodo->derecho = eliminar(nodo->derecho, data);
        else {
            if (!nodo->izquierdo || !nodo->derecho) {
                Nodo* temp = nodo->izquierdo ? nodo->izquierdo : nodo->derecho;
                if (!temp) {
                    temp = nodo;
                    nodo = nullptr;
                } else {
                    *nodo = *temp;
                }
                delete temp;
                num_nodos--;
            } else {
                Nodo* temp = minValor(nodo->derecho);
                nodo->data = temp->data;
                nodo->derecho = eliminar(nodo->derecho, temp->data);
            }
        }

        if (!nodo)
            return nullptr;

        actualizarAltura(nodo);

        int balance = obtenerBalance(nodo);

        if (balance > 1 && obtenerBalance(nodo->izquierdo) >= 0)
            return rotacionDerecha(nodo);

        if (balance > 1 && obtenerBalance(nodo->izquierdo) < 0) {
            nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
            return rotacionDerecha(nodo);
        }

        if (balance < -1 && obtenerBalance(nodo->derecho) <= 0)
            return rotacionIzquierda(nodo);

        if (balance < -1 && obtenerBalance(nodo->derecho) > 0) {
            nodo->derecho = rotacionDerecha(nodo->derecho);
            return rotacionIzquierda(nodo);
        }

        return nodo;
    }

    Nodo* buscarPorPuntaje(Nodo* nodo, double puntaje) {
        if (!nodo) return nullptr;
        if (std::abs(nodo->data.puntaje - puntaje) < 1e-9)
            return nodo;
        if (puntaje < nodo->data.puntaje - 1e-9)
            return buscarPorPuntaje(nodo->izquierdo, puntaje);
        return buscarPorPuntaje(nodo->derecho, puntaje);
    }

    void inorden(Nodo* nodo, std::vector<Postulante>& resultado) {
        if (!nodo) return;
        inorden(nodo->izquierdo, resultado);
        resultado.push_back(nodo->data);
        inorden(nodo->derecho, resultado);
    }

    void inordenInverso(Nodo* nodo, std::vector<Postulante>& resultado) {
        if (!nodo) return;
        inordenInverso(nodo->derecho, resultado);
        resultado.push_back(nodo->data);
        inordenInverso(nodo->izquierdo, resultado);
    }

    bool verificarBalance(Nodo* nodo) {
        if (!nodo) return true;
        int balance = obtenerBalance(nodo);
        if (std::abs(balance) > 1) return false;
        return verificarBalance(nodo->izquierdo) &&
               verificarBalance(nodo->derecho);
    }

    void destruir(Nodo* nodo) {
        if (!nodo) return;
        destruir(nodo->izquierdo);
        destruir(nodo->derecho);
        delete nodo;
    }

public:
    AVLTree() : raiz(nullptr), num_nodos(0), contador_rotaciones(0) {}

    ~AVLTree() { destruir(raiz); }

    void insertar(const Postulante& data) {
        raiz = insertar(raiz, data);
    }

    bool eliminar(double puntaje) {
        Nodo* encontrado = buscarPorPuntaje(raiz, puntaje);
        if (!encontrado) return false;
        raiz = eliminar(raiz, encontrado->data);
        return true;
    }

    bool eliminar(double puntaje, const std::string& codigo) {
        Postulante temp(codigo, "", "", puntaje);
        raiz = eliminar(raiz, temp);
        return true;
    }

    Postulante* buscar(double puntaje) {
        Nodo* resultado = buscarPorPuntaje(raiz, puntaje);
        return resultado ? &(resultado->data) : nullptr;
    }

    std::vector<Postulante> inorden() {
        std::vector<Postulante> resultado;
        inorden(raiz, resultado);
        return resultado;
    }

    std::vector<Postulante> inordenInverso() {
        std::vector<Postulante> resultado;
        inordenInverso(raiz, resultado);
        return resultado;
    }

    int obtenerAltura() { return obtenerAltura(raiz); }
    int obtenerNumNodos() { return num_nodos; }
    int obtenerContadorRotaciones() { return contador_rotaciones; }

    bool verificarBalanceGlobal() {
        return verificarBalance(raiz);
    }

    void imprimirRanking(int top = 20) {
        auto ranking = inordenInverso();
        std::cout << "\n=== TOP " << top << " RANKING PRONABEC ===\n";
        std::cout << std::left << std::setw(15) << "CODIGO"
                  << std::setw(25) << "NOMBRE"
                  << std::setw(15) << "REGION"
                  << std::setw(10) << "PUNTAJE\n";
        std::cout << std::string(65, '-') << "\n";
        for (int i = 0; i < std::min(top, (int)ranking.size()); i++) {
            auto& p = ranking[i];
            std::cout << std::left << std::setw(15) << p.codigo
                      << std::setw(25) << p.nombre
                      << std::setw(15) << p.region
                      << std::right << std::setw(8) << std::fixed
                      << std::setprecision(2) << p.puntaje << "\n";
        }
    }
};

#endif

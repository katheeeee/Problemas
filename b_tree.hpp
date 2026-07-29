#ifndef B_TREE_HPP
#define B_TREE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

struct Libro {
    std::string isbn;
    std::string titulo;
    std::string autor;
    int anio;

    Libro() : isbn(""), titulo(""), autor(""), anio(0) {}
    Libro(std::string i, std::string t, std::string a, int an)
        : isbn(i), titulo(t), autor(a), anio(an) {}
};

class BTree {
private:
    struct Nodo {
        std::vector<int> claves;
        std::vector<Libro> datos;
        std::vector<Nodo*> hijos;
        bool es_hoja;
        int num_claves;

        Nodo(bool hoja, int t)
            : es_hoja(hoja), num_claves(0) {
            claves.resize(2 * t - 1);
            datos.resize(2 * t - 1);
            hijos.resize(2 * t);
        }
    };

    int t;
    Nodo* raiz;
    int num_nodos;

    void destruir(Nodo* nodo) {
        if (!nodo) return;
        if (!nodo->es_hoja) {
            for (int i = 0; i <= nodo->num_claves; i++)
                destruir(nodo->hijos[i]);
        }
        delete nodo;
    }

    void dividirHijo(Nodo* padre, int idx, Nodo* hijo) {
        Nodo* nuevo = new Nodo(hijo->es_hoja, t);
        nuevo->num_claves = t - 1;
        num_nodos++;

        for (int j = 0; j < t - 1; j++) {
            nuevo->claves[j] = hijo->claves[j + t];
            nuevo->datos[j] = hijo->datos[j + t];
        }

        if (!hijo->es_hoja) {
            for (int j = 0; j < t; j++)
                nuevo->hijos[j] = hijo->hijos[j + t];
        }

        hijo->num_claves = t - 1;

        for (int j = padre->num_claves; j >= idx + 1; j--)
            padre->hijos[j + 1] = padre->hijos[j];

        padre->hijos[idx + 1] = nuevo;

        for (int j = padre->num_claves - 1; j >= idx; j--) {
            padre->claves[j + 1] = padre->claves[j];
            padre->datos[j + 1] = padre->datos[j];
        }

        padre->claves[idx] = hijo->claves[t - 1];
        padre->datos[idx] = hijo->datos[t - 1];
        padre->num_claves++;
    }

    void insertarNoLleno(Nodo* nodo, int clave, const Libro& libro) {
        int i = nodo->num_claves - 1;

        if (nodo->es_hoja) {
            while (i >= 0 && clave < nodo->claves[i]) {
                nodo->claves[i + 1] = nodo->claves[i];
                nodo->datos[i + 1] = nodo->datos[i];
                i--;
            }
            nodo->claves[i + 1] = clave;
            nodo->datos[i + 1] = libro;
            nodo->num_claves++;
        } else {
            while (i >= 0 && clave < nodo->claves[i])
                i--;
            i++;
            if (nodo->hijos[i]->num_claves == 2 * t - 1) {
                dividirHijo(nodo, i, nodo->hijos[i]);
                if (clave > nodo->claves[i])
                    i++;
            }
            insertarNoLleno(nodo->hijos[i], clave, libro);
        }
    }

    void inorden(Nodo* nodo, std::vector<std::pair<int, Libro>>& resultado) {
        if (!nodo) return;
        int i;
        for (i = 0; i < nodo->num_claves; i++) {
            if (!nodo->es_hoja)
                inorden(nodo->hijos[i], resultado);
            resultado.push_back({nodo->claves[i], nodo->datos[i]});
        }
        if (!nodo->es_hoja)
            inorden(nodo->hijos[i], resultado);
    }

    Nodo* buscar(Nodo* nodo, int clave) {
        if (!nodo) return nullptr;
        int i = 0;
        while (i < nodo->num_claves && clave > nodo->claves[i])
            i++;
        if (i < nodo->num_claves && clave == nodo->claves[i])
            return nodo;
        if (nodo->es_hoja)
            return nullptr;
        return buscar(nodo->hijos[i], clave);
    }

    int altura(Nodo* nodo) {
        if (!nodo) return 0;
        int h = 1;
        Nodo* actual = nodo;
        while (!actual->es_hoja) {
            actual = actual->hijos[0];
            h++;
        }
        return h;
    }

public:
    BTree(int orden) : t(orden), num_nodos(0) {
        raiz = new Nodo(true, t);
        num_nodos = 1;
    }

    ~BTree() { destruir(raiz); }

    void insertar(int clave, const Libro& libro) {
        if (raiz->num_claves == 2 * t - 1) {
            Nodo* nueva_raiz = new Nodo(false, t);
            nueva_raiz->hijos[0] = raiz;
            dividirHijo(nueva_raiz, 0, raiz);
            int i = 0;
            if (clave > nueva_raiz->claves[0])
                i++;
            insertarNoLleno(nueva_raiz->hijos[i], clave, libro);
            raiz = nueva_raiz;
            num_nodos++;
        } else {
            insertarNoLleno(raiz, clave, libro);
        }
    }

    bool buscar(int clave) {
        return buscar(raiz, clave) != nullptr;
    }

    std::vector<std::pair<int, Libro>> inorden() {
        std::vector<std::pair<int, Libro>> resultado;
        inorden(raiz, resultado);
        return resultado;
    }

    int obtenerAltura() { return altura(raiz); }
    int obtenerNumNodos() { return num_nodos; }
};

#endif

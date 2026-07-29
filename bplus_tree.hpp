#ifndef BPLUS_TREE_HPP
#define BPLUS_TREE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

struct RegistroBibliografico {
    std::string codigo;
    std::string titulo;
    std::string autor;
    int anio;

    RegistroBibliografico() : codigo(""), titulo(""), autor(""), anio(0) {}
    RegistroBibliografico(std::string c, std::string t, std::string a, int an)
        : codigo(c), titulo(t), autor(a), anio(an) {}
};

class BPlusTree {
private:
    int orden;

    struct Nodo {
        bool es_hoja;
        std::vector<int> claves;
        std::vector<RegistroBibliografico> datos;
        std::vector<Nodo*> hijos;
        Nodo* siguiente;

        Nodo(bool hoja, int t)
            : es_hoja(hoja), siguiente(nullptr) {
            claves.reserve(2 * t);
            datos.reserve(2 * t);
            hijos.reserve(2 * t + 1);
        }
    };

    Nodo* raiz;
    Nodo* primera_hoja;

    int indiceInsercion(Nodo* nodo, int clave) {
        int i = 0;
        while (i < (int)nodo->claves.size() && nodo->claves[i] < clave)
            i++;
        return i;
    }

    void insertarEnHoja(Nodo* hoja, int clave, const RegistroBibliografico& reg) {
        int pos = indiceInsercion(hoja, clave);
        hoja->claves.insert(hoja->claves.begin() + pos, clave);
        hoja->datos.insert(hoja->datos.begin() + pos, reg);
    }

    void insertarEnNodoInterno(Nodo* nodo, int clave, Nodo* hijo) {
        int pos = indiceInsercion(nodo, clave);
        nodo->claves.insert(nodo->claves.begin() + pos, clave);
        nodo->hijos.insert(nodo->hijos.begin() + pos + 1, hijo);
    }

    Nodo* dividirHoja(Nodo* hoja, int& clave_promovida) {
        Nodo* nueva = new Nodo(true, orden);
        int medio = hoja->claves.size() / 2;

        nueva->claves.assign(hoja->claves.begin() + medio, hoja->claves.end());
        nueva->datos.assign(hoja->datos.begin() + medio, hoja->datos.end());
        hoja->claves.resize(medio);
        hoja->datos.resize(medio);

        clave_promovida = nueva->claves.front();

        nueva->siguiente = hoja->siguiente;
        hoja->siguiente = nueva;

        return nueva;
    }

    Nodo* dividirInterno(Nodo* nodo, int& clave_promovida) {
        Nodo* nuevo = new Nodo(false, orden);
        int medio = nodo->claves.size() / 2;

        clave_promovida = nodo->claves[medio];

        nuevo->claves.assign(nodo->claves.begin() + medio + 1, nodo->claves.end());
        nuevo->hijos.assign(nodo->hijos.begin() + medio + 1, nodo->hijos.end());
        nodo->claves.resize(medio);
        nodo->hijos.resize(medio + 1);

        return nuevo;
    }

    Nodo* insertarRec(Nodo* nodo, int clave, const RegistroBibliografico& reg, int& clave_promovida, Nodo*& nuevo_hijo) {
        if (nodo->es_hoja) {
            insertarEnHoja(nodo, clave, reg);
            if ((int)nodo->claves.size() >= 2 * orden) {
                nuevo_hijo = dividirHoja(nodo, clave_promovida);
                return nodo;
            }
            clave_promovida = -1;
            nuevo_hijo = nullptr;
            return nodo;
        }

        int i = 0;
        while (i < (int)nodo->claves.size() && clave >= nodo->claves[i])
            i++;

        int clave_temp;
        Nodo* nuevo_temp = nullptr;
        insertarRec(nodo->hijos[i], clave, reg, clave_temp, nuevo_temp);

        if (nuevo_temp) {
            insertarEnNodoInterno(nodo, clave_temp, nuevo_temp);
            if ((int)nodo->claves.size() >= 2 * orden) {
                nuevo_hijo = dividirInterno(nodo, clave_promovida);
                return nodo;
            }
        }

        clave_promovida = -1;
        nuevo_hijo = nullptr;
        return nodo;
    }

public:
    BPlusTree(int t) : orden(t), raiz(nullptr), primera_hoja(nullptr) {}

    ~BPlusTree() {
        destruir(raiz);
    }

    void insertar(int clave, const RegistroBibliografico& reg) {
        if (!raiz) {
            raiz = new Nodo(true, orden);
            raiz->claves.push_back(clave);
            raiz->datos.push_back(reg);
            primera_hoja = raiz;
            return;
        }

        int clave_promovida;
        Nodo* nuevo_hijo = nullptr;
        Nodo* resultado = insertarRec(raiz, clave, reg, clave_promovida, nuevo_hijo);

        if (nuevo_hijo) {
            Nodo* nueva_raiz = new Nodo(false, orden);
            nueva_raiz->claves.push_back(clave_promovida);
            nueva_raiz->hijos.push_back(resultado);
            nueva_raiz->hijos.push_back(nuevo_hijo);
            raiz = nueva_raiz;
        }
    }

    std::vector<RegistroBibliografico> consultarRango(int inicio, int fin) {
        std::vector<RegistroBibliografico> resultado;
        Nodo* actual = primera_hoja;
        while (actual) {
            for (int i = 0; i < (int)actual->claves.size(); i++) {
                if (actual->claves[i] >= inicio && actual->claves[i] <= fin)
                    resultado.push_back(actual->datos[i]);
                else if (actual->claves[i] > fin)
                    return resultado;
            }
            actual = actual->siguiente;
        }
        return resultado;
    }

    int obtenerAltura() {
        if (!raiz) return 0;
        int h = 1;
        Nodo* actual = raiz;
        while (!actual->es_hoja) {
            if (!actual->hijos.empty())
                actual = actual->hijos[0];
            else break;
            h++;
        }
        return h;
    }

private:
    void destruir(Nodo* nodo) {
        if (!nodo) return;
        if (!nodo->es_hoja) {
            for (auto* h : nodo->hijos)
                destruir(h);
        }
        delete nodo;
    }
};

#endif

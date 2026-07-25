// ---------------------------------------------------------------------------
// biblioteca.hpp - Arbol B en C++17
// Practica N 13 - Indexacion del Catalogo de la Biblioteca Central UNA-PUNO
//
// Implementa NodoB con gestion automatica de memoria mediante
// std::vector<std::unique_ptr<NodoB>>, busqueda, insercion con split
// (incluye raiz llena) y ELIMINACION completa con los 3 casos
// (hoja, nodo interno con predecesor/sucesor/fusion, y
// redistribucion/fusion desde hermanos).
// ---------------------------------------------------------------------------
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <stdexcept>

struct Libro {
    std::string codigoTopo, titulo, autor;
    bool disponible = true;
};

struct NodoB {
    std::vector<std::string> claves;
    std::vector<Libro> libros;
    std::vector<std::unique_ptr<NodoB>> hijos;
    bool esHoja = true;
};

class ArbolBBiblioteca {
    int t;
    std::unique_ptr<NodoB> raiz;
    long contador = 0;

    // ---------------------------------------------------------------- //
    // Insercion
    // ---------------------------------------------------------------- //
    void split(NodoB* padre, int i, NodoB* y) {
        auto z = std::make_unique<NodoB>();
        z->esHoja = y->esHoja;
        z->claves.assign(y->claves.begin() + t, y->claves.end());
        z->libros.assign(y->libros.begin() + t, y->libros.end());
        if (!y->esHoja) {
            for (size_t k = t; k < y->hijos.size(); k++)
                z->hijos.push_back(std::move(y->hijos[k]));
        }

        std::string claveMedia = y->claves[t - 1];
        Libro libroMedio = y->libros[t - 1];

        y->claves.resize(t - 1);
        y->libros.resize(t - 1);
        if (!y->esHoja) y->hijos.resize(t);

        padre->hijos.insert(padre->hijos.begin() + i + 1, std::move(z));
        padre->claves.insert(padre->claves.begin() + i, claveMedia);
        padre->libros.insert(padre->libros.begin() + i, libroMedio);
    }

    void insertarNoLleno(NodoB* nodo, const std::string& codigo, Libro lib) {
        int i = (int)nodo->claves.size() - 1;
        if (nodo->esHoja) {
            nodo->claves.push_back("");
            nodo->libros.push_back(Libro{});
            while (i >= 0 && codigo < nodo->claves[i]) {
                nodo->claves[i + 1] = nodo->claves[i];
                nodo->libros[i + 1] = nodo->libros[i];
                i--;
            }
            nodo->claves[i + 1] = codigo;
            nodo->libros[i + 1] = std::move(lib);
        } else {
            while (i >= 0 && codigo < nodo->claves[i]) i--;
            i++;
            if ((int)nodo->hijos[i]->claves.size() == 2 * t - 1) {
                split(nodo, i, nodo->hijos[i].get());
                if (codigo > nodo->claves[i]) i++;
            }
            insertarNoLleno(nodo->hijos[i].get(), codigo, std::move(lib));
        }
    }

    // ---------------------------------------------------------------- //
    // Busqueda (interna, retorna nodo + indice)
    // ---------------------------------------------------------------- //
    const NodoB* buscarNodo(const NodoB* n, const std::string& codigo, int& idx) const {
        int i = 0;
        while (i < (int)n->claves.size() && codigo > n->claves[i]) i++;
        if (i < (int)n->claves.size() && n->claves[i] == codigo) {
            idx = i;
            return n;
        }
        if (n->esHoja) return nullptr;
        return buscarNodo(n->hijos[i].get(), codigo, idx);
    }

    // ---------------------------------------------------------------- //
    // Eliminacion (3 casos, igual que la version Python)
    // ---------------------------------------------------------------- //
    void eliminarRec(NodoB* nodo, const std::string& codigo) {
        int i = 0;
        while (i < (int)nodo->claves.size() && codigo > nodo->claves[i]) i++;

        if (i < (int)nodo->claves.size() && nodo->claves[i] == codigo) {
            if (nodo->esHoja) {                       // Caso 1
                nodo->claves.erase(nodo->claves.begin() + i);
                nodo->libros.erase(nodo->libros.begin() + i);
            } else {                                    // Caso 2
                eliminarInterno(nodo, i);
            }
        } else {
            if (nodo->esHoja)
                throw std::out_of_range("Codigo no encontrado: " + codigo);

            bool enUltimo = (i == (int)nodo->claves.size());
            if ((int)nodo->hijos[i]->claves.size() == t - 1)
                llenar(nodo, i);

            if (enUltimo && i > (int)nodo->claves.size())
                eliminarRec(nodo->hijos[i - 1].get(), codigo);
            else
                eliminarRec(nodo->hijos[i].get(), codigo);
        }
    }

    void eliminarInterno(NodoB* nodo, int i) {
        std::string codigo = nodo->claves[i];
        if ((int)nodo->hijos[i]->claves.size() >= t) {           // Caso 2a
            auto [pred, lib] = obtenerPredecesor(nodo, i);
            nodo->claves[i] = pred;
            nodo->libros[i] = lib;
            eliminarRec(nodo->hijos[i].get(), pred);
        } else if ((int)nodo->hijos[i + 1]->claves.size() >= t) { // Caso 2b
            auto [suc, lib] = obtenerSucesor(nodo, i);
            nodo->claves[i] = suc;
            nodo->libros[i] = lib;
            eliminarRec(nodo->hijos[i + 1].get(), suc);
        } else {                                                    // Caso 2c
            fusionar(nodo, i);
            eliminarRec(nodo->hijos[i].get(), codigo);
        }
    }

    std::pair<std::string, Libro> obtenerPredecesor(NodoB* nodo, int i) {
        NodoB* cur = nodo->hijos[i].get();
        while (!cur->esHoja) cur = cur->hijos.back().get();
        return {cur->claves.back(), cur->libros.back()};
    }

    std::pair<std::string, Libro> obtenerSucesor(NodoB* nodo, int i) {
        NodoB* cur = nodo->hijos[i + 1].get();
        while (!cur->esHoja) cur = cur->hijos.front().get();
        return {cur->claves.front(), cur->libros.front()};
    }

    // Fusiona hijos[i] y hijos[i+1] con la clave nodo->claves[i] en medio.
    void fusionar(NodoB* nodo, int i) {
        NodoB* hijo = nodo->hijos[i].get();
        NodoB* hermano = nodo->hijos[i + 1].get();

        hijo->claves.push_back(nodo->claves[i]);
        hijo->libros.push_back(nodo->libros[i]);

        for (auto& c : hermano->claves) hijo->claves.push_back(c);
        for (auto& l : hermano->libros) hijo->libros.push_back(l);
        if (!hijo->esHoja) {
            for (auto& h : hermano->hijos) hijo->hijos.push_back(std::move(h));
        }

        nodo->claves.erase(nodo->claves.begin() + i);
        nodo->libros.erase(nodo->libros.begin() + i);
        nodo->hijos.erase(nodo->hijos.begin() + i + 1);
    }

    // Asegura que hijos[i] tenga >= t claves antes de descender.
    void llenar(NodoB* nodo, int i) {
        if (i != 0 && (int)nodo->hijos[i - 1]->claves.size() >= t) {
            prestarDeAnterior(nodo, i);
        } else if (i != (int)nodo->claves.size() &&
                   (int)nodo->hijos[i + 1]->claves.size() >= t) {
            prestarDeSiguiente(nodo, i);
        } else {
            if (i != (int)nodo->claves.size())
                fusionar(nodo, i);
            else
                fusionar(nodo, i - 1);
        }
    }

    void prestarDeAnterior(NodoB* nodo, int i) {
        NodoB* hijo = nodo->hijos[i].get();
        NodoB* hermano = nodo->hijos[i - 1].get();

        hijo->claves.insert(hijo->claves.begin(), nodo->claves[i - 1]);
        hijo->libros.insert(hijo->libros.begin(), nodo->libros[i - 1]);
        if (!hijo->esHoja) {
            hijo->hijos.insert(hijo->hijos.begin(), std::move(hermano->hijos.back()));
            hermano->hijos.pop_back();
        }
        nodo->claves[i - 1] = hermano->claves.back();
        nodo->libros[i - 1] = hermano->libros.back();
        hermano->claves.pop_back();
        hermano->libros.pop_back();
    }

    void prestarDeSiguiente(NodoB* nodo, int i) {
        NodoB* hijo = nodo->hijos[i].get();
        NodoB* hermano = nodo->hijos[i + 1].get();

        hijo->claves.push_back(nodo->claves[i]);
        hijo->libros.push_back(nodo->libros[i]);
        if (!hijo->esHoja) {
            hijo->hijos.push_back(std::move(hermano->hijos.front()));
            hermano->hijos.erase(hermano->hijos.begin());
        }
        nodo->claves[i] = hermano->claves.front();
        nodo->libros[i] = hermano->libros.front();
        hermano->claves.erase(hermano->claves.begin());
        hermano->libros.erase(hermano->libros.begin());
    }

    void inOrderRec(const NodoB* n, std::vector<std::string>& salida) const {
        for (size_t i = 0; i < n->claves.size(); i++) {
            if (!n->esHoja) inOrderRec(n->hijos[i].get(), salida);
            salida.push_back(n->claves[i]);
        }
        if (!n->esHoja) inOrderRec(n->hijos.back().get(), salida);
    }

public:
    explicit ArbolBBiblioteca(int orden = 50)
        : t(orden), raiz(std::make_unique<NodoB>()) {
        if (orden < 2) throw std::invalid_argument("t debe ser >= 2");
    }

    void insertar(const std::string& codigo, Libro lib) {
        if ((int)raiz->claves.size() == 2 * t - 1) {
            auto s = std::make_unique<NodoB>();
            s->esHoja = false;
            s->hijos.push_back(std::move(raiz));
            split(s.get(), 0, s->hijos[0].get());
            raiz = std::move(s);
        }
        insertarNoLleno(raiz.get(), codigo, std::move(lib));
        contador++;
    }

    const Libro* buscar(const std::string& codigo) const {
        int idx = -1;
        const NodoB* n = buscarNodo(raiz.get(), codigo, idx);
        return n ? &n->libros[idx] : nullptr;
    }

    void eliminar(const std::string& codigo) {
        if (!buscar(codigo))
            throw std::out_of_range("Codigo no encontrado: " + codigo);
        eliminarRec(raiz.get(), codigo);
        if (raiz->claves.empty() && !raiz->esHoja) {
            raiz = std::move(raiz->hijos[0]);
        }
        contador--;
    }

    // Altura del arbol (0 = solo la raiz, que es hoja).
    int altura() const {
        int h = 0;
        const NodoB* n = raiz.get();
        while (!n->esHoja) {
            h++;
            n = n->hijos[0].get();
        }
        return h;
    }

    long tamano() const { return contador; }

    std::vector<std::string> inOrder() const {
        std::vector<std::string> salida;
        inOrderRec(raiz.get(), salida);
        return salida;
    }

    const NodoB* raizPtr() const { return raiz.get(); }
};


#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>

struct LibroBP {
    std::string titulo;
    std::string autor;
};

struct Nodo {
    bool esHoja;
    std::vector<std::string> claves;
    std::vector<LibroBP> libros;                 // solo si esHoja
    std::vector<std::unique_ptr<Nodo>> hijos;    // solo si !esHoja
    Nodo* siguiente = nullptr;                   // solo si esHoja (raw, no posee)

    explicit Nodo(bool hoja) : esHoja(hoja) {}
};

class ArbolBMas {
public:
    explicit ArbolBMas(int t = 32) : t_(t) {
        if (t_ < 2) throw std::invalid_argument("t debe ser >= 2");
        raiz_ = std::make_unique<Nodo>(true);
        primeraHoja_ = raiz_.get();
    }

    // Actividad 2: insercion con mantenimiento de la lista enlazada
    void insertar(const std::string& codigo, const LibroBP& libro) {
        auto resultado = ins(raiz_.get(), codigo, libro);
        if (resultado.subio) {
            auto nuevaRaiz = std::make_unique<Nodo>(false);
            nuevaRaiz->claves.push_back(resultado.claveGuia);
            nuevaRaiz->hijos.push_back(std::move(raiz_));
            nuevaRaiz->hijos.push_back(std::move(resultado.hermano));
            raiz_ = std::move(nuevaRaiz);
        }
        ++n_;
    }

    // Actividad 3: busqueda puntual O(log n)
    const LibroBP* buscar(const std::string& codigo) const {
        Nodo* hoja = bajarAHoja(codigo);
        auto it = std::lower_bound(hoja->claves.begin(), hoja->claves.end(), codigo);
        if (it != hoja->claves.end() && *it == codigo) {
            size_t idx = static_cast<size_t>(it - hoja->claves.begin());
            return &hoja->libros[idx];
        }
        return nullptr;
    }

    // Actividad 3: consulta de rango O(log n + k)
    std::vector<LibroBP> rango(const std::string& codMin, const std::string& codMax) const {
        std::vector<LibroBP> resultados;
        if (codMin > codMax) return resultados;
        for (Nodo* h = bajarAHoja(codMin); h; h = h->siguiente) {
            for (size_t i = 0; i < h->claves.size(); ++i) {
                if (h->claves[i] > codMax) return resultados;
                if (h->claves[i] >= codMin) resultados.push_back(h->libros[i]);
            }
        }
        return resultados;
    }

    // Recorrido completo O(n), sin tocar nodos internos
    std::vector<LibroBP> recorrerTodoElCatalogo() const {
        std::vector<LibroBP> resultados;
        for (Nodo* h = primeraHoja_; h; h = h->siguiente) {
            resultados.insert(resultados.end(), h->libros.begin(), h->libros.end());
        }
        return resultados;
    }

    // Actividad 4: eliminacion con fusion/redistribucion
    bool eliminar(const std::string& codigo) {
        std::vector<std::pair<Nodo*, size_t>> camino;
        Nodo* nodo = raiz_.get();
        while (!nodo->esHoja) {
            size_t i = static_cast<size_t>(
                std::upper_bound(nodo->claves.begin(), nodo->claves.end(), codigo) -
                nodo->claves.begin());
            camino.emplace_back(nodo, i);
            nodo = nodo->hijos[i].get();
        }
        auto it = std::lower_bound(nodo->claves.begin(), nodo->claves.end(), codigo);
        if (it == nodo->claves.end() || *it != codigo) return false;

        size_t idx = static_cast<size_t>(it - nodo->claves.begin());
        nodo->claves.erase(nodo->claves.begin() + idx);
        nodo->libros.erase(nodo->libros.begin() + idx);
        --n_;

        if (static_cast<int>(nodo->claves.size()) >= t_ - 1 || nodo == raiz_.get()) {
            return true;
        }
        repararHoja(camino, nodo);
        encogerRaizSiCorresponde();
        return true;
    }

    size_t size() const { return n_; }

    // Verificacion de integridad de la lista enlazada (prueba de estres)
    struct ResultadoVerificacion {
        bool ok;
        std::string error;
        size_t totalRegistros = 0;
    };

    ResultadoVerificacion verificarIntegridad() const {
        std::string anterior;
        bool hayAnterior = false;
        size_t total = 0;
        for (Nodo* h = primeraHoja_; h; h = h->siguiente) {
            for (const auto& k : h->claves) {
                if (hayAnterior && k <= anterior) {
                    return {false, "orden roto en la lista enlazada", total};
                }
                anterior = k;
                hayAnterior = true;
                ++total;
            }
        }
        if (total != n_) {
            return {false, "conteo inconsistente tras recorrer la lista", total};
        }
        return {true, "", total};
    }

private:
    struct ResultadoSplit {
        bool subio = false;
        std::string claveGuia;
        std::unique_ptr<Nodo> hermano;
    };

    int t_;
    std::unique_ptr<Nodo> raiz_;
    Nodo* primeraHoja_;
    size_t n_ = 0;

    Nodo* bajarAHoja(const std::string& codigo) const {
        Nodo* nodo = raiz_.get();
        while (!nodo->esHoja) {
            size_t i = static_cast<size_t>(
                std::upper_bound(nodo->claves.begin(), nodo->claves.end(), codigo) -
                nodo->claves.begin());
            nodo = nodo->hijos[i].get();
        }
        return nodo;
    }

    ResultadoSplit ins(Nodo* nodo, const std::string& codigo, const LibroBP& libro) {
        if (nodo->esHoja) {
            auto it = std::lower_bound(nodo->claves.begin(), nodo->claves.end(), codigo);
            size_t i = static_cast<size_t>(it - nodo->claves.begin());
            if (it != nodo->claves.end() && *it == codigo) {
                nodo->libros[i] = libro;  // actualiza en vez de duplicar
                --n_;  // insertar() incrementara n_ de nuevo al retornar
                return {};
            }
            nodo->claves.insert(nodo->claves.begin() + i, codigo);
            nodo->libros.insert(nodo->libros.begin() + i, libro);
            if (static_cast<int>(nodo->claves.size()) <= 2 * t_ - 1) return {};
            return splitHoja(nodo);
        } else {
            size_t i = static_cast<size_t>(
                std::upper_bound(nodo->claves.begin(), nodo->claves.end(), codigo) -
                nodo->claves.begin());
            auto resultado = ins(nodo->hijos[i].get(), codigo, libro);
            if (!resultado.subio) return {};
            nodo->claves.insert(nodo->claves.begin() + i, resultado.claveGuia);
            nodo->hijos.insert(nodo->hijos.begin() + i + 1, std::move(resultado.hermano));
            if (static_cast<int>(nodo->claves.size()) <= 2 * t_ - 1) return {};
            return splitInterno(nodo);
        }
    }

    ResultadoSplit splitHoja(Nodo* hoja) {
        size_t mitad = hoja->claves.size() / 2;
        auto nueva = std::make_unique<Nodo>(true);
        nueva->claves.assign(hoja->claves.begin() + mitad, hoja->claves.end());
        nueva->libros.assign(hoja->libros.begin() + mitad, hoja->libros.end());
        hoja->claves.resize(mitad);
        hoja->libros.resize(mitad);

        nueva->siguiente = hoja->siguiente;   // CRITICO: preservar la lista
        Nodo* nuevaRaw = nueva.get();
        hoja->siguiente = nuevaRaw;

        ResultadoSplit r;
        r.subio = true;
        r.claveGuia = nueva->claves.front();
        r.hermano = std::move(nueva);
        return r;
    }

    ResultadoSplit splitInterno(Nodo* nodo) {
        size_t mitad = nodo->claves.size() / 2;
        std::string claveMedia = nodo->claves[mitad];

        auto nuevo = std::make_unique<Nodo>(false);
        nuevo->claves.assign(nodo->claves.begin() + mitad + 1, nodo->claves.end());
        for (size_t k = mitad + 1; k < nodo->hijos.size(); ++k) {
            nuevo->hijos.push_back(std::move(nodo->hijos[k]));
        }
        nodo->hijos.resize(mitad + 1);
        nodo->claves.resize(mitad);

        ResultadoSplit r;
        r.subio = true;
        r.claveGuia = claveMedia;
        r.hermano = std::move(nuevo);
        return r;
    }

    void repararHoja(std::vector<std::pair<Nodo*, size_t>>& camino, Nodo* hoja) {
        if (camino.empty()) return;
        auto [padre, idx] = camino.back();
        Nodo* hermIzq = (idx > 0) ? padre->hijos[idx - 1].get() : nullptr;
        Nodo* hermDer = (idx + 1 < padre->hijos.size()) ? padre->hijos[idx + 1].get() : nullptr;

        // 1) redistribuir desde el hermano derecho
        if (hermDer && static_cast<int>(hermDer->claves.size()) > t_ - 1) {
            hoja->claves.push_back(hermDer->claves.front());
            hoja->libros.push_back(hermDer->libros.front());
            hermDer->claves.erase(hermDer->claves.begin());
            hermDer->libros.erase(hermDer->libros.begin());
            padre->claves[idx] = hermDer->claves.front();
            return;
        }
        // 2) redistribuir desde el hermano izquierdo
        if (hermIzq && static_cast<int>(hermIzq->claves.size()) > t_ - 1) {
            hoja->claves.insert(hoja->claves.begin(), hermIzq->claves.back());
            hoja->libros.insert(hoja->libros.begin(), hermIzq->libros.back());
            hermIzq->claves.pop_back();
            hermIzq->libros.pop_back();
            padre->claves[idx - 1] = hoja->claves.front();
            return;
        }
        // 3) fusionar (preferentemente con el hermano derecho)
        if (hermDer) {
            hoja->claves.insert(hoja->claves.end(), hermDer->claves.begin(), hermDer->claves.end());
            hoja->libros.insert(hoja->libros.end(), hermDer->libros.begin(), hermDer->libros.end());
            hoja->siguiente = hermDer->siguiente;  // ¡reparar la lista!
            padre->claves.erase(padre->claves.begin() + idx);
            padre->hijos.erase(padre->hijos.begin() + idx + 1);
        } else if (hermIzq) {
            hermIzq->claves.insert(hermIzq->claves.end(), hoja->claves.begin(), hoja->claves.end());
            hermIzq->libros.insert(hermIzq->libros.end(), hoja->libros.begin(), hoja->libros.end());
            hermIzq->siguiente = hoja->siguiente;  // ¡reparar la lista!
            padre->claves.erase(padre->claves.begin() + idx - 1);
            padre->hijos.erase(padre->hijos.begin() + idx);
        }

        camino.pop_back();
        if (!camino.empty() && padre != raiz_.get() &&
            static_cast<int>(padre->claves.size()) < t_ - 1) {
            repararInterno(camino, padre);
        }
    }

    void repararInterno(std::vector<std::pair<Nodo*, size_t>>& camino, Nodo* nodo) {
        if (camino.empty()) return;
        auto [padre, idx] = camino.back();
        Nodo* hermIzq = (idx > 0) ? padre->hijos[idx - 1].get() : nullptr;
        Nodo* hermDer = (idx + 1 < padre->hijos.size()) ? padre->hijos[idx + 1].get() : nullptr;

        if (hermDer && static_cast<int>(hermDer->claves.size()) > t_ - 1) {
            nodo->claves.push_back(padre->claves[idx]);
            padre->claves[idx] = hermDer->claves.front();
            hermDer->claves.erase(hermDer->claves.begin());
            nodo->hijos.push_back(std::move(hermDer->hijos.front()));
            hermDer->hijos.erase(hermDer->hijos.begin());
            return;
        }
        if (hermIzq && static_cast<int>(hermIzq->claves.size()) > t_ - 1) {
            nodo->claves.insert(nodo->claves.begin(), padre->claves[idx - 1]);
            padre->claves[idx - 1] = hermIzq->claves.back();
            hermIzq->claves.pop_back();
            nodo->hijos.insert(nodo->hijos.begin(), std::move(hermIzq->hijos.back()));
            hermIzq->hijos.pop_back();
            return;
        }
        if (hermDer) {
            nodo->claves.push_back(padre->claves[idx]);
            nodo->claves.insert(nodo->claves.end(), hermDer->claves.begin(), hermDer->claves.end());
            for (auto& h : hermDer->hijos) nodo->hijos.push_back(std::move(h));
            padre->claves.erase(padre->claves.begin() + idx);
            padre->hijos.erase(padre->hijos.begin() + idx + 1);
        } else if (hermIzq) {
            hermIzq->claves.push_back(padre->claves[idx - 1]);
            hermIzq->claves.insert(hermIzq->claves.end(), nodo->claves.begin(), nodo->claves.end());
            for (auto& h : nodo->hijos) hermIzq->hijos.push_back(std::move(h));
            padre->claves.erase(padre->claves.begin() + idx - 1);
            padre->hijos.erase(padre->hijos.begin() + idx);
        }

        camino.pop_back();
        if (!camino.empty() && padre != raiz_.get() &&
            static_cast<int>(padre->claves.size()) < t_ - 1) {
            repararInterno(camino, padre);
        }
    }

    void encogerRaizSiCorresponde() {
        // MEJORA sobre la guia original: si la raiz interna queda con
        // un unico hijo, ese hijo pasa a ser la nueva raiz.
        while (!raiz_->esHoja && raiz_->hijos.size() == 1) {
            raiz_ = std::move(raiz_->hijos[0]);
        }
        primeraHoja_ = raiz_.get();
        while (!primeraHoja_->esHoja) primeraHoja_ = primeraHoja_->hijos[0].get();
    }
};

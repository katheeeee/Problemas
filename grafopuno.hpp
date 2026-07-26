
#pragma once
#include <vector>
#include <string>
#include <array>
#include <stdexcept>
#include <algorithm>

using Arista = std::pair<int, int>;

class GrafoPuno {
    int n;
    std::vector<std::vector<Arista>> adj;
    int naristas = 0;

    static constexpr std::array<const char*, 14> ciudades = {
        "Puno", "Juliaca", "Ilave", "Desaguadero", "Yunguyo", "Juli",
        "Lampa", "Azangaro", "Huancane", "Moho", "Putina", "Ayaviri",
        "Macusani", "Sandia"
    };

    void validarVertice(int u) const {
        if (u < 0 || u >= n)
            throw std::out_of_range("Vertice fuera de rango: " + std::to_string(u));
    }

public:
    explicit GrafoPuno(int n) : n(n), adj(n) {
        if (n <= 0) throw std::invalid_argument("n debe ser positivo");
    }

    bool existeArista(int u, int v) const {
        validarVertice(u);
        validarVertice(v);
        return std::any_of(adj[u].begin(), adj[u].end(),
                            [v](const Arista& a) { return a.first == v; });
    }

    void agregarArista(int u, int v, int peso) {
        validarVertice(u);
        validarVertice(v);
        if (u == v)
            throw std::invalid_argument("No se permiten bucles: (" + std::to_string(u) + "," + std::to_string(u) + ")");
        if (existeArista(u, v))
            throw std::invalid_argument("Arista duplicada: (" + std::to_string(u) + "," + std::to_string(v) + ")");
        adj[u].push_back({v, peso});
        adj[v].push_back({u, peso});
        naristas++;
    }

    const std::vector<Arista>& vecinos(int u) const {
        validarVertice(u);
        return adj[u];
    }

    int grado(int u) const {
        validarVertice(u);
        return (int)adj[u].size();
    }

    double densidad() const {
        if (n < 2) return 0.0;
        return 2.0 * naristas / (double)(n * (n - 1));
    }

    std::string nombreCiudad(int u) const {
        validarVertice(u);
        return u < (int)ciudades.size() ? ciudades[u] : std::to_string(u);
    }

    int numVertices() const { return n; }
    int numAristas() const { return naristas; }
};

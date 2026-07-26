#include "grafopuno.hpp"
#include <queue>
#include <unordered_set>
#include <chrono>
#include <iostream>
#include <tuple>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

std::vector<std::vector<int>> componentesConexas(
        const GrafoPuno& g, const std::unordered_set<int>& excluidos = {}) {
    std::vector<std::vector<int>> componentes;
    std::unordered_set<int> visitados = excluidos;

    for (int inicio = 0; inicio < g.numVertices(); inicio++) {
        if (visitados.count(inicio)) continue;
        std::vector<int> comp;
        std::queue<int> cola;
        cola.push(inicio);
        visitados.insert(inicio);
        while (!cola.empty()) {
            int u = cola.front();
            cola.pop();
            comp.push_back(u);
            for (auto& [v, _] : g.vecinos(u)) {
                if (!visitados.count(v) && !excluidos.count(v)) {
                    visitados.insert(v);
                    cola.push(v);
                }
            }
        }
        componentes.push_back(comp);
    }
    return componentes;
}

void imprimirComponentes(const GrafoPuno& g, const std::vector<std::vector<int>>& comp) {
    for (auto& c : comp) {
        std::cout << "  [";
        for (size_t i = 0; i < c.size(); i++) {
            std::cout << g.nombreCiudad(c[i]);
            if (i + 1 < c.size()) std::cout << ", ";
        }
        std::cout << "]\n";
    }
}

int main() {
    auto t0 = Clock::now();

    GrafoPuno g(14);
    std::vector<std::tuple<int, int, int>> rutas = {
        {0, 1, 44}, {0, 2, 55}, {0, 5, 80}, {1, 6, 37}, {1, 7, 70}, {1, 11, 90},
        {2, 3, 50}, {2, 4, 45}, {3, 4, 25}, {5, 4, 60}, {7, 8, 95}, {7, 10, 110},
        {7, 11, 75}, {8, 9, 40}, {11, 12, 140}, {11, 13, 180}
    };
    for (auto& [u, v, p] : rutas) g.agregarArista(u, v, p);

    double msConstruccion = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();

    std::cout << "V=" << g.numVertices() << " E=" << g.numAristas()
              << " densidad=" << g.densidad() << '\n';

    for (int u = 0; u < g.numVertices(); u++)
        std::cout << "  " << g.nombreCiudad(u) << " grado=" << g.grado(u) << '\n';

    auto t1 = Clock::now();
    auto comp = componentesConexas(g);
    double msComponentesNormal = std::chrono::duration<double, std::milli>(Clock::now() - t1).count();

    std::cout << "\nComponentes (sin bloqueos): " << comp.size() << '\n';
    imprimirComponentes(g, comp);

    auto t2 = Clock::now();
    auto compLluvia = componentesConexas(g, {12, 13});
    double msComponentesLluvia = std::chrono::duration<double, std::milli>(Clock::now() - t2).count();

    std::cout << "\nComponentes (Macusani/Sandia bloqueadas): " << compLluvia.size() << '\n';
    imprimirComponentes(g, compLluvia);

    std::cout << "\nTiempo construccion del grafo: " << msConstruccion << " ms\n";
    std::cout << "Tiempo componentes (sin bloqueos): " << msComponentesNormal << " ms\n";
    std::cout << "Tiempo componentes (con bloqueos): " << msComponentesLluvia << " ms\n";

    return 0;
}

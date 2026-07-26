

#include "catalogo.hpp"
#include <iostream>
#include <map>
#include <set>
#include <random>
#include <chrono>
#include <iomanip>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

static std::string generarCodigo(std::mt19937& rng) {
    std::uniform_int_distribution<int> d(0, 999);
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%03d.%03d", d(rng), d(rng));
    return std::string(buf);
}

struct ResultadoBench {
    double msBPlus, msMap, msSet;
    size_t k;  // numero de resultados encontrados (debe coincidir en las 3)
};

static ResultadoBench compararEstructuras(int n, const std::string& codMin,
                                           const std::string& codMax) {
    std::mt19937 rng(1234 + n);
    std::set<std::string> unicos;
    while (static_cast<int>(unicos.size()) < n) unicos.insert(generarCodigo(rng));
    std::vector<std::string> codigos(unicos.begin(), unicos.end());
    std::shuffle(codigos.begin(), codigos.end(), rng);

    // --- Construir las tres estructuras con los MISMOS datos ---
    ArbolBMas arbolBP(/*t=*/32);
    std::map<std::string, LibroBP> mapaBTree;      // simula indice tipo B-Tree
    std::set<std::string> conjuntoAVL;             // simula AVL/RBT (solo claves)

    for (size_t i = 0; i < codigos.size(); ++i) {
        LibroBP libro{"Obra " + std::to_string(i), "Autor " + std::to_string(i % 500)};
        arbolBP.insertar(codigos[i], libro);
        mapaBTree[codigos[i]] = libro;
        conjuntoAVL.insert(codigos[i]);
    }

    // --- B+ : un solo descenso + recorrido de la lista enlazada ---
    auto t0 = Clock::now();
    auto resBP = arbolBP.rango(codMin, codMax);
    double msBP = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();

    // --- "B-Tree" (std::map): equivalente a redescender por cada
    //     resultado, ya que no existe lista enlazada entre hojas; se
    //     usa lower_bound + avance de iterador nodo a nodo ---
    t0 = Clock::now();
    size_t kMap = 0;
    for (auto it = mapaBTree.lower_bound(codMin); it != mapaBTree.end() && it->first <= codMax; ++it) {
        ++kMap;
    }
    double msMap = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();

    // --- "AVL / RBT" (std::set): mismo patron, sin datos asociados ---
    t0 = Clock::now();
    size_t kSet = 0;
    for (auto it = conjuntoAVL.lower_bound(codMin); it != conjuntoAVL.end() && *it <= codMax; ++it) {
        ++kSet;
    }
    double msSet = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();

    return {msBP, msMap, msSet, resBP.size()};
}

int main() {
    std::cout << std::fixed << std::setprecision(4);
    std::cout << std::left << std::setw(12) << "N"
              << std::setw(14) << "k (result.)"
              << std::setw(16) << "B+ (ms)"
              << std::setw(18) << "B-Tree/map (ms)"
              << std::setw(16) << "AVL/set (ms)"
              << "Ganador\n";
    std::cout << std::string(90, '-') << "\n";

    for (int n : {1000, 10000, 80000}) {
        auto r = compararEstructuras(n, "004.000", "004.999");
        double minimo = std::min({r.msBPlus, r.msMap, r.msSet});
        std::string ganador = (minimo == r.msBPlus) ? "B+"
                              : (minimo == r.msMap)  ? "B-Tree"
                                                      : "AVL/RBT";
        std::cout << std::left << std::setw(12) << n
                  << std::setw(14) << r.k
                  << std::setw(16) << r.msBPlus
                  << std::setw(18) << r.msMap
                  << std::setw(16) << r.msSet
                  << ganador << "\n";
    }
    return 0;
}

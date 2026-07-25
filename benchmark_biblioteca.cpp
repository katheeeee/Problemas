
// ---------------------------------------------------------------------------
// benchmark_biblioteca.cpp - Comparacion de orden t: RAM (t=2) vs disco (t=100)
// para 1,000,000 de registros (C++17)
//
// Corrige el bug de la version original de la guia: aqui se expone
// altura() en la clase ArbolBBiblioteca y se usa directamente, en vez
// de la expresion rota "altura(arbol.buscar(...) ? nullptr : nullptr)".
//
// Compilar:
//   g++ -std=c++17 -O2 -Wall -o benchmark benchmark_biblioteca.cpp
// Ejecutar:
//   ./benchmark
// ---------------------------------------------------------------------------
#include "biblioteca.hpp"
#include <chrono>
#include <random>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

using Clock = std::chrono::high_resolution_clock;

struct ResultadoBenchmark {
    int t;
    int n;
    double msInsercion;
    double msBusqueda;
    int altura;
    int clavesMax;
    int accesosDisco; // altura + 1 (raiz incluida)
};

ResultadoBenchmark benchmarkOrden(int t, int n, unsigned semilla = 42) {
    ArbolBBiblioteca arbol(t);
    std::mt19937 rng(semilla);

    std::vector<std::string> codigos;
    codigos.reserve(n);
    for (int i = 0; i < n; i++) codigos.push_back(std::to_string(100000 + i));
    std::shuffle(codigos.begin(), codigos.end(), rng);

    auto t0 = Clock::now();
    for (auto& c : codigos) arbol.insertar(c, Libro{c, "T", "A", true});
    double msIns = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();

    // Medir tiempo de 1000 busquedas aleatorias
    std::shuffle(codigos.begin(), codigos.end(), rng);
    auto t1 = Clock::now();
    for (int i = 0; i < 1000; i++) arbol.buscar(codigos[i]);
    double msBus = std::chrono::duration<double, std::milli>(Clock::now() - t1).count();

    int h = arbol.altura();
    return ResultadoBenchmark{t, n, msIns, msBus, h, 2 * t - 1, h + 1};
}

int main() {
    const int n = 1'000'000;
    std::cout << "Comparando ordenes t para n=" << n << " registros:\n\n";
    std::cout << std::left
              << std::setw(6) << "t"
              << std::setw(14) << "insercion(ms)"
              << std::setw(16) << "busq.1000(ms)"
              << std::setw(9) << "altura"
              << std::setw(14) << "claves_max"
              << "accesos_disco\n";
    std::cout << std::string(70, '-') << "\n";

    for (int t : {2, 10, 50, 100, 500}) {
        auto r = benchmarkOrden(t, n);
        std::cout << std::left
                  << std::setw(6) << r.t
                  << std::setw(14) << std::fixed << std::setprecision(2) << r.msInsercion
                  << std::setw(16) << r.msBusqueda
                  << std::setw(9) << r.altura
                  << std::setw(14) << r.clavesMax
                  << r.accesosDisco << "\n";
    }

    return 0;
}

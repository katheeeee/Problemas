// catalogo.cpp -- Demo del Arbol B+ (Practica 14)
// Compilar:  g++ -std=c++17 -O2 -Wall -o catalogo catalogo.cpp
// Ejecutar:  ./catalogo

#include "catalogo.hpp"
#include <iostream>
#include <random>
#include <chrono>
#include <set>
#include <cassert>
#include <iomanip>

using Clock = std::chrono::high_resolution_clock;

static std::string generarCodigo(std::mt19937& rng) {
    std::uniform_int_distribution<int> d(0, 999);
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%03d.%03d", d(rng), d(rng));
    return std::string(buf);
}

int main() {
    std::mt19937 rng(42);  // semilla fija, reproducible

    // ---- Actividad 5 / 6: indexar un catalogo simulado ----
    const int N_OBJETIVO = 80000;
    std::set<std::string> unicos;
    while (static_cast<int>(unicos.size()) < N_OBJETIVO) {
        unicos.insert(generarCodigo(rng));
    }
    std::vector<std::string> codigos(unicos.begin(), unicos.end());
    std::shuffle(codigos.begin(), codigos.end(), rng);

    ArbolBMas catalogo(/*t=*/50);
    for (size_t i = 0; i < codigos.size(); ++i) {
        LibroBP libro{"Obra " + std::to_string(i), "Autor " + std::to_string(i % 500)};
        catalogo.insertar(codigos[i], libro);
    }
    std::cout << "Catalogo indexado: " << catalogo.size() << " libros\n";

    // ---- Consulta de rango: seccion 004 (Ciencias de la Computacion) ----
    auto t0 = Clock::now();
    auto resultados = catalogo.rango("004.000", "004.999");
    double ms = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
    std::cout << "Seccion 004 (Computacion): " << resultados.size()
              << " libros encontrados en " << std::fixed << std::setprecision(3) << ms << " ms\n";

    // ---- Consulta de rango mas amplia: 000-099 ----
    t0 = Clock::now();
    auto resultados2 = catalogo.rango("000.000", "099.999");
    double ms2 = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
    std::cout << "Rango 000-099: " << resultados2.size()
              << " libros encontrados en " << ms2 << " ms\n";

    // ---- Verificacion de integridad antes de eliminar ----
    auto v1 = catalogo.verificarIntegridad();
    std::cout << "Verificacion (antes de eliminar): ok=" << std::boolalpha << v1.ok
              << " total=" << v1.totalRegistros << "\n";
    assert(v1.ok);

    // ---- Actividad 4 / prueba de estres: 5,000 eliminaciones aleatorias ----
    std::shuffle(codigos.begin(), codigos.end(), rng);
    t0 = Clock::now();
    for (int i = 0; i < 5000; ++i) {
        bool eliminado = catalogo.eliminar(codigos[i]);
        assert(eliminado);
    }
    double ms3 = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
    std::cout << "\n5,000 eliminaciones ejecutadas en " << ms3 << " ms\n";

    auto v2 = catalogo.verificarIntegridad();
    std::cout << "Verificacion (despues de eliminar): ok=" << v2.ok
              << " total=" << v2.totalRegistros << "\n";
    assert(v2.ok);
    assert(v2.totalRegistros == 75000);

    // ---- Repetir consulta de rango tras las eliminaciones ----
    t0 = Clock::now();
    auto resultados3 = catalogo.rango("004.000", "004.999");
    double ms4 = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
    std::cout << "Seccion 004 (post-eliminacion): " << resultados3.size()
              << " libros en " << ms4 << " ms\n";

    std::cout << "\nTodas las verificaciones pasaron correctamente.\n";
    return 0;
}

// ---------------------------------------------------------------------------
// main_biblioteca.cpp - Simulacion del catalogo de la Biblioteca Central
// UNA-PUNO (C++17)
//
// Compilar:
//   g++ -std=c++17 -O2 -Wall -o biblioteca main_biblioteca.cpp
// Ejecutar:
//   ./biblioteca
// ---------------------------------------------------------------------------
#include "biblioteca.hpp"
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <iomanip>
#include <algorithm>

static std::vector<std::string> generarCodigos(int cantidad, unsigned semilla = 42) {
    std::mt19937 rng(semilla);
    std::uniform_int_distribution<int> d3(0, 999);
    std::uniform_int_distribution<int> letra(0, 25);
    std::uniform_int_distribution<int> d2(1, 99);

    std::set<std::string> unicos;
    while ((int)unicos.size() < cantidad) {
        std::ostringstream oss;
        oss << std::setw(3) << std::setfill('0') << d3(rng) << "."
            << std::setw(3) << std::setfill('0') << d3(rng) << " "
            << char('A' + letra(rng)) << d2(rng);
        unicos.insert(oss.str());
    }
    return std::vector<std::string>(unicos.begin(), unicos.end());
}

int main() {
    std::cout << "=== PRUEBA ARBOL B (C++17) - BIBLIOTECA CENTRAL UNA-PUNO ===\n\n";

    const int N_LIBROS = 80000;
    const int T = 50; // simula bloque de disco real

    ArbolBBiblioteca biblioteca(T);
    std::vector<std::string> codigos = generarCodigos(N_LIBROS);

    std::vector<std::string> barajados = codigos;
    std::mt19937 rng(42);
    std::shuffle(barajados.begin(), barajados.end(), rng);

    std::cout << "Indexando " << N_LIBROS << " volumenes (t=" << T << ")...\n";
    for (size_t i = 0; i < barajados.size(); i++) {
        Libro lib{barajados[i], "Obra " + std::to_string(i),
                   "Autor " + std::to_string(i % 500), true};
        biblioteca.insertar(barajados[i], lib);
    }
    std::cout << "Catalogo indexado. Altura del arbol: " << biblioteca.altura() << "\n";

    // 500 prestamos (eliminacion temporal)
    std::vector<std::string> prestamos = codigos;
    std::shuffle(prestamos.begin(), prestamos.end(), rng);
    prestamos.resize(500);
    for (auto& c : prestamos) biblioteca.eliminar(c);
    std::cout << prestamos.size() << " prestamos procesados (libros retirados del indice).\n";

    // 300 devoluciones
    std::vector<std::string> devoluciones(prestamos.begin(), prestamos.begin() + 300);
    for (auto& c : devoluciones) {
        Libro lib{c, "(reinsertado)", "(reinsertado)", true};
        biblioteca.insertar(c, lib);
    }
    std::cout << devoluciones.size() << " devoluciones procesadas (libros reinsertados).\n";

    // Verificacion: los 200 no devueltos no deben encontrarse; el resto si.
    std::set<std::string> noDevueltos(prestamos.begin() + 300, prestamos.end());
    int esperados = 0, encontrados = 0;
    for (auto& c : codigos) {
        if (noDevueltos.count(c)) continue;
        esperados++;
        if (biblioteca.buscar(c)) encontrados++;
    }
    std::cout << "Libros indexados verificables tras la simulacion: "
              << encontrados << "/" << esperados << "\n";

    // Prueba de estres: 10,000 eliminaciones + 10,000 inserciones aleatorias
    std::cout << "\n=== PRUEBA DE ESTRES ===\n";
    std::vector<std::string> presentes;
    for (auto& c : codigos) if (!noDevueltos.count(c)) presentes.push_back(c);

    std::shuffle(presentes.begin(), presentes.end(), rng);
    int nOps = std::min<int>(10000, (int)presentes.size());
    std::vector<std::string> aEliminar(presentes.begin(), presentes.begin() + nOps);

    std::set<std::string> conjuntoEsperado(presentes.begin(), presentes.end());

    for (auto& c : aEliminar) biblioteca.eliminar(c);
    for (size_t i = 0; i < aEliminar.size(); i++) {
        Libro lib{aEliminar[i], "Reinsertado " + std::to_string(i), "Estres", true};
        biblioteca.insertar(aEliminar[i], lib);
    }

    auto orden = biblioteca.inOrder();
    bool ordenadoCorrectamente = std::is_sorted(orden.begin(), orden.end());
    std::set<std::string> conjuntoFinal(orden.begin(), orden.end());
    bool sinPerdida = (conjuntoFinal == conjuntoEsperado);

    std::cout << "Operaciones realizadas: " << aEliminar.size() << " eliminaciones + "
              << aEliminar.size() << " inserciones\n";
    std::cout << "Orden preservado (in-order): " << (ordenadoCorrectamente ? "true" : "false") << "\n";
    std::cout << "Catalogo sin perdida de registros: " << (sinPerdida ? "true" : "false") << "\n";
    std::cout << "Altura final del arbol: " << biblioteca.altura() << "\n";

    return 0;
}

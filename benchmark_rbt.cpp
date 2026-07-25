#include "rbt.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include <algorithm>

std::string generarDni(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(10000000, 99999999);
    return std::to_string(dist(rng));
}

int main() {
    std::vector<int> tamanos = {1000, 10000, 100000, 500000};
    std::mt19937 rng(12345);

    for (int n : tamanos) {
        ArbolElectoral arbol;
        std::vector<std::string> dnis;
        dnis.reserve(n);

        auto t0 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < n; ++i) {
            std::string dni = generarDni(rng);
            dnis.push_back(dni);
            Votante v{dni, "Votante_" + std::to_string(i), "Facultad_" + std::to_string(i % 10), true};
            arbol.insertar(v);
        }
        auto t1 = std::chrono::high_resolution_clock::now();

        std::shuffle(dnis.begin(), dnis.end(), rng);
        int encontrados = 0;
        for (const auto& dni : dnis) {
            if (arbol.buscar(dni) != nullptr) ++encontrados;
        }
        auto t2 = std::chrono::high_resolution_clock::now();

        std::shuffle(dnis.begin(), dnis.end(), rng);
        int mitad = n / 2;
        int eliminados = 0;
        for (int i = 0; i < mitad; ++i) {
            if (arbol.eliminar(dnis[i])) ++eliminados;
        }
        auto t3 = std::chrono::high_resolution_clock::now();

        bool valido = arbol.esRBTValido();
        int alturaReal = arbol.altura();
        double alturaTeorica = 2.0 * std::log2(n + 1);

        double msInsertar = std::chrono::duration<double, std::milli>(t1 - t0).count();
        double msBuscar = std::chrono::duration<double, std::milli>(t2 - t1).count();
        double msEliminar = std::chrono::duration<double, std::milli>(t3 - t2).count();

        std::cout << "==== N = " << n << " ====\n";
        std::cout << "Insercion:  " << msInsertar << " ms\n";
        std::cout << "Busqueda:   " << msBuscar << " ms (" << encontrados << "/" << n << " encontrados)\n";
        std::cout << "Eliminacion: " << msEliminar << " ms (" << eliminados << "/" << mitad << " eliminados)\n";
        std::cout << "Altura real: " << alturaReal << " | limite teorico 2*log2(n+1): " << alturaTeorica << "\n";
        std::cout << "Propiedades RBT validas: " << (valido ? "Si" : "No") << "\n\n";
    }

    return 0;
}

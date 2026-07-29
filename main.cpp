#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <map>
#include <algorithm>
#include <numeric>
#include <cstdlib>

#include "avl_tree.hpp"
#include "red_black_tree.hpp"
#include "b_tree.hpp"
#include "bplus_tree.hpp"
#include "graph.hpp"

// ============================================================
// PARTE II: AVL - SISTEMA DE RANKING PRONABEC (25 pts)
// ============================================================
void parteII_AVL_PRONABEC() {
    std::cout << "\n========================================================\n";
    std::cout << "PARTE II: AVL - SISTEMA DE RANKING PRONABEC\n";
    std::cout << "========================================================\n";

    AVLTree arbol;

    std::mt19937 gen(42);
    std::uniform_real_distribution<double> dist(0.0, 100.0);
    std::vector<std::string> regiones = {
        "Puno", "Cusco", "Arequipa", "Tacna", "Moquegua",
        "Lima", "Junin", "Loreto", "Piura", "La Libertad"
    };

    std::vector<std::string> nombres_ejemplo = {
        "Garcia", "Rodriguez", "Martinez", "Lopez", "Hernandez",
        "Gonzalez", "Perez", "Sanchez", "Rojas", "Diaz",
        "Torres", "Ramirez", "Flores", "Castillo", "Vargas",
        "Reyes", "Cruz", "Ortiz", "Morales", "Chavez",
        "Quispe", "Mamani", "Condori", "Huaman", "Paco"
    };

    std::vector<Postulante*> postulantes_creados;

    std::cout << "Insertando 5000 postulantes...\n";
    for (int i = 1; i <= 5000; i++) {
        char buf[20];
        std::snprintf(buf, sizeof(buf), "PRNB-%05d", i);
        std::string codigo(buf);

        std::string nombre = nombres_ejemplo[gen() % nombres_ejemplo.size()]
                           + " " + nombres_ejemplo[(gen() % nombres_ejemplo.size())];
        std::string region = regiones[gen() % regiones.size()];
        double puntaje = std::round(dist(gen) * 100.0) / 100.0;

        Postulante p(codigo, nombre, region, puntaje);
        arbol.insertar(p);

        if (i <= 5 || i % 1000 == 0) {
            std::cout << "  Insertado: " << codigo << " -> " << std::fixed
                      << std::setprecision(2) << puntaje << "\n";
        }
    }

    std::cout << "\n--- ESTADISTICAS DEL ARBOL ---\n";
    std::cout << "Total nodos: " << arbol.obtenerNumNodos() << "\n";
    std::cout << "Altura final: " << arbol.obtenerAltura() << "\n";
    std::cout << "Rotaciones totales: " << arbol.obtenerContadorRotaciones() << "\n";
    std::cout << "Altura teorica minima (log2 n): "
              << std::ceil(std::log2(5001)) << "\n";
    std::cout << "Balance: " << (arbol.verificarBalanceGlobal() ? "OK" : "ERROR") << "\n";

    auto todos = arbol.inordenInverso();
    std::vector<Postulante*> primeros;
    std::vector<Postulante*> ultimos;
    for (int i = 0; i < std::min(3, (int)todos.size()); i++)
        primeros.push_back(&todos[i]);
    for (int i = std::max(0, (int)todos.size() - 3); i < (int)todos.size(); i++)
        ultimos.push_back(&todos[i]);

    std::cout << "\n--- BUSQUEDAS ---\n";
    double buscar_inicio = primeros.empty() ? 95.0 : primeros[0]->puntaje;
    double buscar_fin = ultimos.empty() ? 5.0 : ultimos[0]->puntaje;
    double buscar_inexistente = 999.99;

    Postulante* res1 = arbol.buscar(buscar_inicio);
    std::cout << "Buscar puntaje " << std::fixed << std::setprecision(2) << buscar_inicio
              << " (inicio ranking): " << (res1 ? res1->codigo : "NO ENCONTRADO") << "\n";

    Postulante* res2 = arbol.buscar(buscar_fin);
    std::cout << "Buscar puntaje " << buscar_fin
              << " (final ranking): " << (res2 ? res2->codigo : "NO ENCONTRADO") << "\n";

    Postulante* res3 = arbol.buscar(buscar_inexistente);
    std::cout << "Buscar puntaje " << buscar_inexistente
              << " (inexistente): " << (res3 ? res3->codigo : "NO ENCONTRADO (correcto)") << "\n";

    std::cout << "\n--- ELIMINACION DE 10 POSTULANTES ALEATORIOS ---\n";
    std::mt19937 gen2(123);
    std::uniform_int_distribution<int> idx_dist(0, (int)todos.size() - 1);

    for (int i = 1; i <= 10; i++) {
        int idx = idx_dist(gen2);
        double puntaje_elim = todos[idx].puntaje;
        bool ok = arbol.eliminar(todos[idx].puntaje, todos[idx].codigo);
        bool balanceado = arbol.verificarBalanceGlobal();
        std::cout << "  Eliminacion " << i << ": puntaje=" << std::fixed
                  << std::setprecision(2) << puntaje_elim
                  << " -> " << (ok ? "OK" : "FALLO")
                  << " | Balance: " << (balanceado ? "OK" : "ERROR")
                  << " | Altura: " << arbol.obtenerAltura() << "\n";
    }

    arbol.imprimirRanking(20);
}

// ============================================================
// PARTE III: AVL vs ROJO-NEGRO (15 pts)
// ============================================================
void parteIII_AVL_vs_RojoNegro() {
    std::cout << "\n========================================================\n";
    std::cout << "PARTE III: AVL vs ROJO-NEGRO (PADRON ELECTORAL)\n";
    std::cout << "========================================================\n";

    const int NUM_VOTANTES = 14000;
    const int NUM_BUSQUEDAS = 500;

    std::mt19937 gen(99);
    std::uniform_int_distribution<int> dist_busqueda(0, NUM_VOTANTES - 1);

    std::vector<int> codigos_disponibles(900000);
    std::iota(codigos_disponibles.begin(), codigos_disponibles.end(), 100000);
    std::shuffle(codigos_disponibles.begin(), codigos_disponibles.end(), gen);

    std::vector<std::string> nombres_votantes = {
        "Garcia", "Rodriguez", "Martinez", "Lopez", "Hernandez",
        "Gonzalez", "Perez", "Sanchez", "Rojas", "Diaz",
        "Quispe", "Mamani", "Condori", "Huaman", "Paco"
    };

    std::vector<std::pair<int, Votante>> datos_votantes;
    for (int i = 0; i < NUM_VOTANTES; i++) {
        int cod = codigos_disponibles[i];
        char dni_buf[20];
        std::snprintf(dni_buf, sizeof(dni_buf), "%08d", cod % 100000000);
        std::string nombre = nombres_votantes[gen() % nombres_votantes.size()]
                           + " " + nombres_votantes[(gen() % nombres_votantes.size())];
        datos_votantes.push_back({cod, Votante(std::to_string(cod), nombre, dni_buf)});
    }

    std::cout << "\n--- MEDICION: ARBOL AVL ---\n";
    AVLTree arbol_avl;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_VOTANTES; i++) {
        arbol_avl.insertar(Postulante(
            std::to_string(datos_votantes[i].first),
            datos_votantes[i].second.nombre,
            "Puno",
            (double)datos_votantes[i].first
        ));
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double insercion_avl_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    auto t2 = std::chrono::high_resolution_clock::now();
    int encontrados_avl = 0;
    for (int i = 0; i < NUM_BUSQUEDAS; i++) {
        int idx = dist_busqueda(gen);
        int cod = datos_votantes[idx].first;
        auto* res = arbol_avl.buscar((double)cod);
        if (res) encontrados_avl++;
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    double busqueda_avl_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();

    int alt_avl = arbol_avl.obtenerAltura();
    int rot_avl = arbol_avl.obtenerContadorRotaciones();

    std::cout << "  Tiempo insercion: " << insercion_avl_ms << " ms\n";
    std::cout << "  Tiempo busqueda (" << NUM_BUSQUEDAS << "): " << busqueda_avl_ms << " ms\n";
    std::cout << "  Rotaciones: " << rot_avl << "\n";
    std::cout << "  Altura final: " << alt_avl << "\n";

    std::cout << "\n--- MEDICION: ARBOL ROJO-NEGRO ---\n";
    RedBlackTree arbol_rb;
    auto t4 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_VOTANTES; i++) {
        arbol_rb.insertar(datos_votantes[i].first,
                          datos_votantes[i].second);
    }
    auto t5 = std::chrono::high_resolution_clock::now();
    double insercion_rb_ms = std::chrono::duration<double, std::milli>(t5 - t4).count();

    auto t6 = std::chrono::high_resolution_clock::now();
    int encontrados_rb = 0;
    for (int i = 0; i < NUM_BUSQUEDAS; i++) {
        int idx = dist_busqueda(gen);
        int cod = datos_votantes[idx].first;
        auto* res = arbol_rb.buscar(cod);
        if (res) encontrados_rb++;
    }
    auto t7 = std::chrono::high_resolution_clock::now();
    double busqueda_rb_ms = std::chrono::duration<double, std::milli>(t7 - t6).count();

    int alt_rb = arbol_rb.obtenerAltura();
    int rot_rb = arbol_rb.obtenerContadorRotaciones();

    std::cout << "  Tiempo insercion: " << insercion_rb_ms << " ms\n";
    std::cout << "  Tiempo busqueda (" << NUM_BUSQUEDAS << "): " << busqueda_rb_ms << " ms\n";
    std::cout << "  Rotaciones: " << rot_rb << "\n";
    std::cout << "  Altura final: " << alt_rb << "\n";

    std::cout << "\n--- TABLA COMPARATIVA ---\n";
    std::cout << std::left << std::setw(35) << "Metrica"
              << std::setw(20) << "AVL"
              << "Rojo-Negro\n";
    std::cout << std::string(75, '-') << "\n";
    std::cout << std::left << std::setw(35) << "Tiempo insercion (ms)"
              << std::right << std::setw(12) << std::fixed << std::setprecision(2) << insercion_avl_ms
              << std::setw(12) << std::fixed << std::setprecision(2) << insercion_rb_ms << "\n";
    std::cout << std::left << std::setw(35) << "Tiempo busqueda (ms)"
              << std::right << std::setw(12) << std::fixed << std::setprecision(2) << busqueda_avl_ms
              << std::setw(12) << std::fixed << std::setprecision(2) << busqueda_rb_ms << "\n";
    std::cout << std::left << std::setw(35) << "Numero de rotaciones"
              << std::right << std::setw(12) << rot_avl
              << std::setw(12) << rot_rb << "\n";
    std::cout << std::left << std::setw(35) << "Altura final"
              << std::right << std::setw(12) << alt_avl
              << std::setw(12) << alt_rb << "\n";
}

// ============================================================
// PARTE IV: ARBOLES B Y B+ (10 pts)
// ============================================================
void parteIV_ArbolesB() {
    std::cout << "\n========================================================\n";
    std::cout << "PARTE IV: ARBOLES B Y B+ (CATALOGO BIBLIOTECA)\n";
    std::cout << "========================================================\n";

    std::cout << "\n--- ARBOL B (orden t=50) ---\n";
    BTree btree(50);

    std::mt19937 gen(777);
    for (int i = 0; i < 80000; i++) {
        int cod = 10000 + i;
        char titulo[50];
        std::snprintf(titulo, sizeof(titulo), "Libro_%05d", i + 1);
        char autor[50];
        std::snprintf(autor, sizeof(autor), "Autor_%03d", (gen() % 500) + 1);
        int anio = 1950 + (gen() % 75);
        btree.insertar(cod, Libro(std::to_string(cod), titulo, autor, anio));
    }

    auto btree_datos = btree.inorden();
    std::cout << "Registros insertados: 80000\n";
    std::cout << "Registros verificados (inorden): " << btree_datos.size() << "\n";
    std::cout << "Altura del arbol: " << btree.obtenerAltura() << "\n";
    std::cout << "Nodos en el arbol: " << btree.obtenerNumNodos() << "\n";

    std::cout << "\n--- ARBOL B+ (orden t=50) ---\n";
    BPlusTree bplus(50);

    for (int i = 0; i < 80000; i++) {
        int cod = 10000 + i;
        char titulo[50];
        std::snprintf(titulo, sizeof(titulo), "Libro_%05d", i + 1);
        char autor[50];
        std::snprintf(autor, sizeof(autor), "Autor_%03d", (gen() % 500) + 1);
        bplus.insertar(cod, RegistroBibliografico(
            std::to_string(cod), titulo, autor, 1950 + (gen() % 75))
        );
    }

    std::cout << "Registros insertados: 80000\n";
    std::cout << "Altura del arbol: " << bplus.obtenerAltura() << "\n";

    std::cout << "\n--- CONSULTA POR RANGO (B+) ---\n";
    std::cout << "Buscando codigos entre 010000 y 015000...\n";
    auto resultados = bplus.consultarRango(10000, 15000);
    std::cout << "Registros encontrados en rango: " << resultados.size() << "\n";
    if (!resultados.empty()) {
        std::cout << "Primeros 5 resultados:\n";
        for (int i = 0; i < std::min(5, (int)resultados.size()); i++) {
            std::cout << "  " << resultados[i].codigo << " - "
                      << resultados[i].titulo << "\n";
        }
    }
}

// ============================================================
// PARTE V: GRAFOS - RED VIAL DEL ALTIPLANO (10 pts)
// ============================================================
void parteV_Grafos() {
    std::cout << "\n========================================================\n";
    std::cout << "PARTE V: GRAFOS - RED VIAL DEL ALTIPLANO\n";
    std::cout << "========================================================\n";

    Grafo g(8);
    g.agregarVertice(0, "Puno");
    g.agregarVertice(1, "Juliaca");
    g.agregarVertice(2, "Acora");
    g.agregarVertice(3, "Lampa");
    g.agregarVertice(4, "Huancane");
    g.agregarVertice(5, "Ilave");
    g.agregarVertice(6, "Cabanilla");
    g.agregarVertice(7, "Moho");

    g.agregarArista(0, 1, 44);
    g.agregarArista(0, 2, 22);
    g.agregarArista(1, 3, 34);
    g.agregarArista(1, 4, 50);
    g.agregarArista(2, 5, 31);
    g.agregarArista(3, 6, 20);
    g.agregarArista(4, 7, 40);
    g.agregarArista(5, 7, 38);

    g.imprimirAdyacencia();

    std::cout << "\n--- BFS DESDE PUNO ---\n";
    auto bfs_orden = g.BFS(0);

    std::cout << "\nOrden BFS: ";
    for (int v : bfs_orden)
        std::cout << g.obtenerNombre(v) << " ";
    std::cout << "\n";

    std::cout << "\n--- DFS (PILA) DESDE PUNO ---\n";
    auto dfs_orden = g.DFS(0);
    std::cout << "\nOrden DFS (pila): ";
    for (int v : dfs_orden)
        std::cout << g.obtenerNombre(v) << " ";
    std::cout << "\n";

    std::cout << "\n--- DFS RECURSIVO DESDE PUNO ---\n";
    auto dfs_rec_orden = g.DFSRecursivoWrapper(0);
    std::cout << "\nOrden DFS recursivo: ";
    for (int v : dfs_rec_orden)
        std::cout << g.obtenerNombre(v) << " ";
    std::cout << "\n";

    g.imprimirDijkstraCompleto(0);

    std::cout << "\n--- COMPLEJIDAD ---\n";
    std::cout << "|V| = 8, |E| = 8\n";
    std::cout << "BFS: O(|V| + |E|) = O(16)\n";
    std::cout << "DFS: O(|V| + |E|) = O(16)\n";
    std::cout << "Dijkstra: O((|V| + |E|) log |V|) ≈ O(16 log 8)\n";
}

// ============================================================
// MAIN
// ============================================================
int main() {
    std::cout << std::fixed << std::setprecision(2);



    parteII_AVL_PRONABEC();
    parteIII_AVL_vs_RojoNegro();
    parteIV_ArbolesB();
    parteV_Grafos();

    std::cout << "\n========================================================\n";
    std::cout << "FIN DEL PROGRAMA - TODAS LAS PARTES COMPLETADAS\n";
    std::cout << "========================================================\n";

    return 0;
}

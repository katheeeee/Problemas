#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <algorithm>
#include <limits>
#include <iomanip>

struct Arista {
    int destino;
    double peso;
    Arista(int d, double p) : destino(d), peso(p) {}
};

class Grafo {
private:
    int num_vertices;
    std::vector<std::vector<Arista>> lista_adyacencia;
    std::vector<std::string> nombres;

public:
    Grafo(int n) : num_vertices(n), lista_adyacencia(n), nombres(n) {}

    void agregarVertice(int idx, const std::string& nombre) {
        if (idx >= 0 && idx < num_vertices)
            nombres[idx] = nombre;
    }

    void agregarArista(int origen, int destino, double peso) {
        if (origen >= 0 && origen < num_vertices &&
            destino >= 0 && destino < num_vertices) {
            lista_adyacencia[origen].emplace_back(destino, peso);
            lista_adyacencia[destino].emplace_back(origen, peso);
        }
    }

    std::vector<int> BFS(int origen) {
        std::vector<bool> visitado(num_vertices, false);
        std::vector<int> orden;
        std::vector<int> distancia(num_vertices, -1);
        std::queue<int> cola;

        visitado[origen] = true;
        distancia[origen] = 0;
        cola.push(origen);

        while (!cola.empty()) {
            int actual = cola.front();
            cola.pop();
            orden.push_back(actual);

            std::cout << "Visitado: " << nombres[actual]
                      << " (distancia: " << distancia[actual] << " saltos)\n";

            for (const auto& arista : lista_adyacencia[actual]) {
                if (!visitado[arista.destino]) {
                    visitado[arista.destino] = true;
                    distancia[arista.destino] = distancia[actual] + 1;
                    cola.push(arista.destino);
                }
            }
        }

        return orden;
    }

    std::vector<int> DFS(int origen) {
        std::vector<bool> visitado(num_vertices, false);
        std::vector<int> orden;
        std::stack<int> pila;

        pila.push(origen);

        while (!pila.empty()) {
            int actual = pila.top();
            pila.pop();

            if (!visitado[actual]) {
                visitado[actual] = true;
                orden.push_back(actual);
                std::cout << "Visitado: " << nombres[actual] << "\n";

                for (int i = (int)lista_adyacencia[actual].size() - 1; i >= 0; i--) {
                    if (!visitado[lista_adyacencia[actual][i].destino])
                        pila.push(lista_adyacencia[actual][i].destino);
                }
            }
        }

        return orden;
    }

    void DFSRecursivo(int actual, std::vector<bool>& visitado,
                      std::vector<int>& orden) {
        visitado[actual] = true;
        orden.push_back(actual);
        std::cout << "Visitado: " << nombres[actual] << "\n";

        for (const auto& arista : lista_adyacencia[actual]) {
            if (!visitado[arista.destino])
                DFSRecursivo(arista.destino, visitado, orden);
        }
    }

    std::vector<int> DFSRecursivoWrapper(int origen) {
        std::vector<bool> visitado(num_vertices, false);
        std::vector<int> orden;
        std::cout << "DFS RECURSIVO:\n";
        DFSRecursivo(origen, visitado, orden);
        return orden;
    }

    struct DijkstraResultado {
        std::vector<double> distancias;
        std::vector<int> predecesores;
    };

    DijkstraResultado Dijkstra(int origen) {
        std::vector<double> dist(num_vertices, std::numeric_limits<double>::infinity());
        std::vector<int> prev(num_vertices, -1);
        std::vector<bool> procesado(num_vertices, false);

        dist[origen] = 0.0;

        using Par = std::pair<double, int>;
        std::priority_queue<Par, std::vector<Par>, std::greater<Par>> pq;
        pq.push({0.0, origen});

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            if (procesado[u]) continue;
            procesado[u] = true;

            for (const auto& arista : lista_adyacencia[u]) {
                int v = arista.destino;
                double peso = arista.peso;

                if (dist[u] + peso < dist[v]) {
                    dist[v] = dist[u] + peso;
                    prev[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        return {dist, prev};
    }

    void imprimirCamino(const std::vector<int>& prev, int destino) {
        if (destino < 0 || destino >= num_vertices) return;
        std::vector<int> camino;
        for (int v = destino; v != -1; v = prev[v])
            camino.push_back(v);
        std::reverse(camino.begin(), camino.end());
        for (int i = 0; i < (int)camino.size(); i++) {
            std::cout << nombres[camino[i]];
            if (i < (int)camino.size() - 1)
                std::cout << " -> ";
        }
        std::cout << "\n";
    }

    void imprimirDijkstraCompleto(int origen) {
        auto [distancias, predecesores] = Dijkstra(origen);

        std::cout << "\n=== DIJKSTRA - CAMINOS MINIMOS DESDE " << nombres[origen] << " ===\n";
        std::cout << std::left << std::setw(15) << "DESTINO"
                  << std::setw(15) << "DISTANCIA"
                  << "RUTA\n";
        std::cout << std::string(60, '-') << "\n";

        for (int i = 0; i < num_vertices; i++) {
            if (i == origen) continue;
            std::cout << std::left << std::setw(15) << nombres[i]
                      << std::right << std::setw(8) << std::fixed
                      << std::setprecision(1) << distancias[i] << " km     ";
            imprimirCamino(predecesores, i);
        }
    }

    void imprimirAdyacencia() {
        std::cout << "\nLISTA DE ADYACENCIA:\n";
        for (int i = 0; i < num_vertices; i++) {
            std::cout << nombres[i] << ": ";
            for (const auto& arista : lista_adyacencia[i]) {
                std::cout << nombres[arista.destino] << "(" << arista.peso << "km) ";
            }
            std::cout << "\n";
        }
    }

    int obtenerNumVertices() { return num_vertices; }
    std::string obtenerNombre(int idx) {
        return (idx >= 0 && idx < num_vertices) ? nombres[idx] : "";
    }
};

#endif

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <stdexcept>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <chrono> // Necesario para el benchmark de rendimiento experimental
#include <random> // Para generar datasets grandes dinámicamente

using namespace std;

namespace una_puno {

enum class EstadoAcademico {
    ACTIVO, EGRESADO, RETIRADO, SUSPENDIDO
};

inline string estadoStr(EstadoAcademico e) {
    switch(e) {
        case EstadoAcademico::ACTIVO:     return "ACTIVO";
        case EstadoAcademico::EGRESADO:    return "EGRESADO";
        case EstadoAcademico::RETIRADO:    return "RETIRADO";
        case EstadoAcademico::SUSPENDIDO:  return "SUSPENDIDO";
        default:                           return "DESCONOCIDO";
    }
}

struct Estudiante {
    int codigo;
    string nombre;
    string escuela;
    float ppa;
    int creditos;
    EstadoAcademico estado;
    string semestre_ingreso;

    Estudiante(int cod, string nom, string esc, float pp, int cred, EstadoAcademico est, string sem)
        : codigo(cod), nombre(move(nom)), escuela(move(esc)), ppa(pp), creditos(cred), estado(est), semestre_ingreso(move(sem))
    {
        if (cod < 10000000 || cod > 29999999)
            throw invalid_argument("Codigo invalido.");
        if (pp < 0.0f || pp > 20.0f)
            throw invalid_argument("PPA fuera de rango [0,20]");
    }

    void print() const {
        cout << left
             << setw(10) << codigo
             << setw(25) << nombre
             << setw(20) << escuela
             << "PPA: " << fixed << setprecision(1) << setw(5) << ppa
             << " " << estadoStr(estado) << '\n';
    }
};

struct NodoBST {
    Estudiante dato;
    unique_ptr<NodoBST> izquierdo;
    unique_ptr<NodoBST> derecho;

    explicit NodoBST(Estudiante e)
        : dato(move(e)), izquierdo(nullptr), derecho(nullptr) {}
};

class ArbolAcademico {
public:
    ArbolAcademico() = default;

    void insertar(Estudiante e) {
        insertar_(raiz, move(e));
    }

    optional<Estudiante> buscar(int codigo) const {
        const NodoBST* nodo = buscar_(raiz.get(), codigo);
        if (nodo) return nodo->dato;
        return nullopt;
    }

    void eliminar(int codigo) {
        if (!buscar(codigo))
            throw runtime_error("Codigo no encontrado.");
        raiz = eliminar_(move(raiz), codigo);
    }

    vector<Estudiante> inOrder() const {
        vector<Estudiante> resultado;
        inOrder_(raiz.get(), resultado);
        return resultado;
    }

    vector<Estudiante> preOrder() const {
        vector<Estudiante> resultado;
        preOrder_(raiz.get(), resultado);
        return resultado;
    }

    vector<Estudiante> postOrder() const {
        vector<Estudiante> resultado;
        postOrder_(raiz.get(), resultado);
        return resultado;
    }

    vector<Estudiante> bfs() const {
        vector<Estudiante> resultado;
        if (!raiz) return resultado;
        queue<const NodoBST*> cola;
        cola.push(raiz.get());
        while (!cola.empty()) {
            const NodoBST* curr = cola.front();
            cola.pop();
            resultado.push_back(curr->dato);
            if (curr->izquierdo) cola.push(curr->izquierdo.get());
            if (curr->derecho) cola.push(curr->derecho.get());
        }
        return resultado;
    }

    int altura() const { return altura_(raiz.get()); }

    void estadisticas() const {
        auto todos = inOrder();
        if (todos.empty()) {
            cout << "No hay registros en el sistema.\n";
            return;
        }
        float suma = 0.0f, mn = 20.0f, mx = 0.0f;
        int activos = 0;
        for (const auto& e : todos) {
            suma += e.ppa;
            if (e.ppa < mn) mn = e.ppa;
            if (e.ppa > mx) mx = e.ppa;
            if (e.estado == EstadoAcademico::ACTIVO) activos++;
        }
        cout << fixed << setprecision(2)
             << "\n=== METRICAS DEL INVARIANTES DEL BST ===\n"
             << " - Total Nodos (n) : " << todos.size() << '\n'
             << " - Altura (h)       : " << altura() << '\n'
             << " - PPA Promedio     : " << suma / todos.size() << '\n'
             << " - PPA Maximo       : " << mx << '\n'
             << " - PPA Minimo       : " << mn << '\n';
    }

private:
    unique_ptr<NodoBST> raiz;

    void insertar_(unique_ptr<NodoBST>& nodo, Estudiante e) {
        if (!nodo) {
            nodo = make_unique<NodoBST>(move(e));
            return;
        }
        if (e.codigo < nodo->dato.codigo)
            insertar_(nodo->izquierdo, move(e));
        else if (e.codigo > nodo->dato.codigo)
            insertar_(nodo->derecho, move(e));
        else
            throw runtime_error("Error: Codigo universitario ya existe.");
    }

    unique_ptr<NodoBST> eliminar_(unique_ptr<NodoBST> nodo, int cod) {
        if (!nodo) return nullptr;

        if (cod < nodo->dato.codigo)
            nodo->izquierdo = eliminar_(move(nodo->izquierdo), cod);
        else if (cod > nodo->dato.codigo)
            nodo->derecho = eliminar_(move(nodo->derecho), cod);
        else {
            // Caso 1 y Caso 2: Un solo hijo o ninguno
            if (!nodo->izquierdo) return move(nodo->derecho);
            if (!nodo->derecho) return move(nodo->izquierdo);

            // Caso 3: Dos hijos (Buscar sucesor en el subárbol derecho)
            NodoBST* suc = minimo_(nodo->derecho.get());
            nodo->dato = suc->dato;
            nodo->derecho = eliminar_(move(nodo->derecho), suc->dato.codigo);
        }
        return nodo;
    }

    const NodoBST* buscar_(const NodoBST* n, int cod) const {
        if (!n || n->dato.codigo == cod) return n;
        if (cod < n->dato.codigo) return buscar_(n->izquierdo.get(), cod);
        return buscar_(n->derecho.get(), cod);
    }

    NodoBST* minimo_(NodoBST* n) const {
        while (n->izquierdo) n = n->izquierdo.get();
        return n;
    }

    int altura_(const NodoBST* n) const {
        if (!n) return -1;
        return 1 + max(altura_(n->izquierdo.get()), altura_(n->derecho.get()));
    }

    void inOrder_(const NodoBST* n, vector<Estudiante>& r) const {
        if (!n) return;
        inOrder_(n->izquierdo.get(), r);
        r.push_back(n->dato);
        inOrder_(n->derecho.get(), r);
    }

    void preOrder_(const NodoBST* n, vector<Estudiante>& r) const {
        if (!n) return;
        r.push_back(n->dato);
        preOrder_(n->izquierdo.get(), r);
        preOrder_(n->derecho.get(), r);
    }

    void postOrder_(const NodoBST* n, vector<Estudiante>& r) const {
        if (!n) return;
        postOrder_(n->izquierdo.get(), r);
        postOrder_(n->derecho.get(), r);
        r.push_back(n->dato);
    }
};

} // namespace una_puno

using namespace una_puno;

void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Ejecuta una prueba de estrés para el reporte experimental
void ejecutarBenchmark() {
    cout << "\n--- Ejecutando Benchmark Experimental (Dataset a Gran Escala) ---\n";
    ArbolAcademico arbolPruebas;
    const int TAMANO_DATASET = 50000;

    // Generador aleatorio para evitar degradación total en la prueba promedio
    mt19937 generado(12345);
    uniform_int_distribution<int> distCodigo(10000000, 29999999);

    vector<int> codigosInsertados;
    codigosInsertados.reserve(TAMANO_DATASET);

    auto inicio = chrono::high_resolution_clock::now();
    for(int i = 0; i < TAMANO_DATASET; ++i) {
        int cod = distCodigo(generado);
        try {
            arbolPruebas.insertar({cod, "Estudiante Benchmark", "Escuela Sede", 14.0f, 100, EstadoAcademico::ACTIVO, "2026-I"});
            codigosInsertados.push_back(cod);
        } catch(...) { i--; } // Reintentar si sale código duplicado
    }
    auto fin = chrono::high_resolution_clock::now();
    double tiempoInsercion = chrono::duration_cast<chrono::microseconds>(fin - inicio).count() / 1000.0;

    // Medición de búsqueda masiva
    inicio = chrono::high_resolution_clock::now();
    for(int cod : codigosInsertados) {
        arbolPruebas.buscar(cod);
    }
    fin = chrono::high_resolution_clock::now();
    double tiempoBusqueda = chrono::duration_cast<chrono::microseconds>(fin - inicio).count() / 1000.0;

    cout << "Resultados de rendimiento en C++17:\n";
    cout << " -> Tamaño del Dataset : " << TAMANO_DATASET << " registros\n";
    cout << " -> Tiempo de Insercion: " << tiempoInsercion << " ms\n";
    cout << " -> Tiempo de Busqueda : " << tiempoBusqueda << " ms\n";
    cout << "Aviso: Copie estos tiempos exactos en su tabla comparativa de la Actividad 13.\n";
}

int main() {
    ArbolAcademico arbol;

    // Carga inicial requerida
    arbol.insertar({20210500, "Mamani Quispe, Juan", "Ing. Sistemas", 15.8f, 120, EstadoAcademico::ACTIVO, "2021-I"});
    arbol.insertar({20210300, "Huanca Apaza, Maria", "Ing. Civil", 14.2f, 110, EstadoAcademico::ACTIVO, "2021-I"});
    arbol.insertar({20210700, "Condori Flores, Pedro", "Medicina", 17.1f, 130, EstadoAcademico::ACTIVO, "2021-I"});
    arbol.insertar({20210100, "Ticona Lupaca, Rosa", "Contabilidad", 12.0f, 90, EstadoAcademico::SUSPENDIDO, "2021-I"});
    arbol.insertar({20210400, "Larico Ccama, Carlos", "Ing. Sistemas", 16.5f, 115, EstadoAcademico::ACTIVO, "2021-I"});
    arbol.insertar({20210600, "Cutipa Vargas, Elena", "Agronomia", 13.7f, 100, EstadoAcademico::ACTIVO, "2021-I"});
    arbol.insertar({20210900, "Pari Choque, Luis", "Ing. Sistemas", 18.3f, 140, EstadoAcademico::EGRESADO, "2021-I"});

    int opcion = 0;
    do {
        cout << "\n=============================================\n";
        cout << "   SISTEMA DE EXPEDIENTES - UNA PUNO (BST)\n";
        cout << "=============================================\n";
        cout << "1. Insertar Nuevo Estudiante\n";
        cout << "2. Buscar Estudiante por Codigo\n";
        cout << "3. Eliminar Estudiante por Codigo\n";
        cout << "4. Ver Todos los Estudiantes Registrados\n";
        cout << "5. Menu de Recorridos Especiales (Pre, Post, BFS)\n";
        cout << "6. Ver Metricas e Invariantes del BST\n";
        cout << "7. Ejecutar Benchmark de Rendimiento (Act. 13)\n";
        cout << "8. Salir\n";
        cout << "Seleccione una opcion: ";

        if (!(cin >> opcion)) {
            cout << "Opcion invalida. Intente de nuevo.\n";
            limpiarBuffer();
            continue;
        }

        switch (opcion) {
            case 1: {
                int cod, cred, estOpt;
                string nom, esc, sem;
                float ppa;

                cout << "\n--- Registrar Nuevo Estudiante ---\n";
                cout << "Codigo (8 digitos): "; cin >> cod;
                limpiarBuffer();
                cout << "Nombre Completo: "; getline(cin, nom);
                cout << "Escuela Profesional: "; getline(cin, esc);
                cout << "PPA (0.0 - 20.0): "; cin >> ppa;
                cout << "Creditos Aprobados: "; cin >> cred;
                cout << "Estado (0:ACTIVO, 1:EGRESADO, 2:RETIRADO, 3:SUSPENDIDO): "; cin >> estOpt;
                limpiarBuffer();
                cout << "Semestre Ingreso: "; getline(cin, sem);

                EstadoAcademico est = EstadoAcademico::ACTIVO;
                if (estOpt == 1) est = EstadoAcademico::EGRESADO;
                else if (estOpt == 2) est = EstadoAcademico::RETIRADO;
                else if (estOpt == 3) est = EstadoAcademico::SUSPENDIDO;

                try {
                    arbol.insertar({cod, nom, esc, ppa, cred, est, sem});
                    cout << ">> Estudiante registrado.\n";
                } catch (const exception& e) {
                    cout << ">> Error al insertar: " << e.what() << '\n';
                }
                break;
            }
            case 2: {
                int cod;
                cout << "\nIngrese el codigo a buscar: "; cin >> cod;
                auto res = arbol.buscar(cod);
                if (res) {
                    cout << ">> Estudiante Encontrado:\n";
                    res->print();
                } else {
                    cout << ">> Codigo " << cod << " no existe.\n";
                }
                break;
            }
            case 3: {
                int cod;
                cout << "\nIngrese el codigo a eliminar: "; cin >> cod;
                try {
                    arbol.eliminar(cod);
                    cout << ">> Registro eliminado correctamente del BST.\n";
                } catch (const exception& e) {
                    cout << ">> Error: " << e.what() << '\n';
                }
                break;
            }
            case 4: {
                cout << "\n--- Estudiantes Registrados (Orden Ascendente In-Order) ---\n";
                auto lista = arbol.inOrder();
                if (lista.empty()) cout << "El arbol esta vacio.\n";
                else { for (const auto& e : lista) e.print(); }
                break;
            }
            case 5: {
                int tipoRecorrido;
                cout << "\n--- Recorridos Algoritmicos ---\n";
                cout << "1. Pre-Order\n";
                cout << "2. Post-Order\n";
                cout << "3. BFS / Por Niveles\n";
                cout << "Seleccione tipo: "; cin >> tipoRecorrido;

                if (tipoRecorrido == 1) {
                    cout << "\n[Recorrido Pre-Order]\n";
                    for (const auto& e : arbol.preOrder()) cout << e.codigo << " ";
                    cout << "\n";
                } else if (tipoRecorrido == 2) {
                    cout << "\n[Recorrido Post-Order]\n";
                    for (const auto& e : arbol.postOrder()) cout << e.codigo << " ";
                    cout << "\n";
                } else if (tipoRecorrido == 3) {
                    cout << "\n[Recorrido por Niveles / BFS]\n";
                    for (const auto& e : arbol.bfs()) cout << e.codigo << " ";
                    cout << "\n";
                } else cout << "Modo no valido.\n";
                break;
            }
            case 6:
                arbol.estadisticas();
                break;
            case 7:
                ejecutarBenchmark();
                break;
            case 8:
                cout << "Saliendo del sistema.\n";
                break;
            default:
                cout << "Opcion no valida.\n";
        }
    } while (opcion != 8);

    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <chrono>
#include <random>
#include <cassert> // Necesario para el uso de assert()
#include <climits> // Necesario para INT_MIN e INT_MAX

using namespace std;

namespace una_puno {

enum class EstadoAcademico {
    ACTIVO, EGRESADO, RETIRADO, SUSPENDIDO
};

inline string obtenerCadenaEstado(EstadoAcademico estado) {
    switch(estado) {
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

    Estudiante(int _codigo, string _nombre, string _escuela, float _ppa, int _creditos, EstadoAcademico _estado, string _semestre) {
        if (_codigo < 10000000 || _codigo > 29999999) {
            throw invalid_argument("Codigo invalido. Debe tener 8 digitos universitarios.");
        }
        if (_ppa < 0.0f || _ppa > 20.0f) {
            throw invalid_argument("PPA fuera de rango valido [0, 20]");
        }

        this->codigo = _codigo;
        this->nombre = _nombre;
        this->escuela = _escuela;
        this->ppa = _ppa;
        this->creditos = _creditos;
        this->estado = _estado;
        this->semestre_ingreso = _semestre;
    }

    void mostrarInformacion() const {
        cout << left
             << setw(10) << codigo
             << setw(25) << nombre
             << setw(20) << escuela
             << "PPA: " << fixed << setprecision(1) << setw(5) << ppa
             << " Estado: " << obtenerCadenaEstado(estado) << '\n';
    }
};

struct NodoBST {
    Estudiante dato;
    NodoBST* izquierdo;
    NodoBST* derecho;

    explicit NodoBST(Estudiante estudianteInput)
        : dato(estudianteInput), izquierdo(nullptr), derecho(nullptr) {}
};

class ArbolAcademico {
public:
    ArbolAcademico() {
        this->raiz = nullptr;
    }

    ~ArbolAcademico() {
        liberarArbol(this->raiz);
    }

    void insertar(Estudiante nuevoEstudiante) {
        this->raiz = insertarAuxiliar(this->raiz, nuevoEstudiante);
    }

    const Estudiante* buscar(int codigoABuscar) const {
        const NodoBST* nodoEncontrado = buscarAuxiliar(this->raiz, codigoABuscar);
        if (nodoEncontrado != nullptr) {
            return &(nodoEncontrado->dato);
        }
        return nullptr;
    }

    void eliminar(int codigoAEliminar) {
        if (buscar(codigoAEliminar) == nullptr) {
            throw runtime_error("Codigo no encontrado en el sistema universitario.");
        }
        this->raiz = eliminarAuxiliar(this->raiz, codigoAEliminar);
    }

    vector<Estudiante> obtenerInOrder() const {
        vector<Estudiante> resultado;
        inOrderAuxiliar(this->raiz, resultado);
        return resultado;
    }

    vector<Estudiante> obtenerPreOrder() const {
        vector<Estudiante> resultado;
        preOrderAuxiliar(this->raiz, resultado);
        return resultado;
    }

    vector<Estudiante> obtenerPostOrder() const {
        vector<Estudiante> resultado;
        postOrderAuxiliar(this->raiz, resultado);
        return resultado;
    }

    vector<Estudiante> obtenerBFS() const {
        vector<Estudiante> resultado;
        if (this->raiz == nullptr) {
            return resultado;
        }

        queue<const NodoBST*> colaNodos;
        colaNodos.push(this->raiz);

        while (colaNodos.empty() == false) {
            const NodoBST* nodoActual = colaNodos.front();
            colaNodos.pop();

            resultado.push_back(nodoActual->dato);

            if (nodoActual->izquierdo != nullptr) {
                colaNodos.push(nodoActual->izquierdo);
            }
            if (nodoActual->derecho != nullptr) {
                colaNodos.push(nodoActual->derecho);
            }
        }
        return resultado;
    }

    int calcularAltura() const {
        return alturaAuxiliar(this->raiz);
    }

    // --- ENLACES PÚBLICOS PARA LAS VERIFICACIONES P3 Y P4 ---
    NodoBST* obtenerRaiz() const {
        return this->raiz;
    }

    int contarNodos(const NodoBST* n) const {
        if (n == nullptr) return 0;
        return 1 + contarNodos(n->izquierdo) + contarNodos(n->derecho);
    }

    bool esArbolBST(const NodoBST* n, int mn = INT_MIN, int mx = INT_MAX) const {
        if (n == nullptr) return true;
        if (n->dato.codigo <= mn || n->dato.codigo >= mx) return false;
        return esArbolBST(n->izquierdo, mn, n->dato.codigo) &&
               esArbolBST(n->derecho, n->dato.codigo, mx);
    }
    // -------------------------------------------------------

    void mostrarEstadisticas() const {
        vector<Estudiante> todosLosEstudiantes = obtenerInOrder();
        if (todosLosEstudiantes.empty()) {
            cout << "No hay registros en el sistema actualmente.\n";
            return;
        }

        float sumaPPA = 0.0f;
        float ppaMinimo = 20.0f;
        float ppaMaximo = 0.0f;
        int estudiantesActivos = 0;

        for (size_t i = 0; i < todosLosEstudiantes.size(); ++i) {
            float ppaActual = todosLosEstudiantes[i].ppa;
            sumaPPA += ppaActual;

            if (ppaActual < ppaMinimo) { ppaMinimo = ppaActual; }
            if (ppaActual > ppaMaximo) { ppaMaximo = ppaActual; }
            if (todosLosEstudiantes[i].estado == EstadoAcademico::ACTIVO) { estudiantesActivos++; }
        }

        cout << fixed << setprecision(2)
             << "\n=== METRICAS E INVARIANTES DEL BST ===\n"
             << " - Total de Nodos en el Arbol : " << todosLosEstudiantes.size() << '\n'
             << " - Altura del Arbol (h)       : " << calcularAltura() << '\n'
             << " - PPA Promedio General       : " << (sumaPPA / todosLosEstudiantes.size()) << '\n'
             << " - PPA Maximo Registrado      : " << ppaMaximo << '\n'
             << " - PPA Minimo Registrado      : " << ppaMinimo << '\n'
             << " - Total Alumnos Activos      : " << estudiantesActivos << '\n';
    }

private:
    NodoBST* raiz;

    void liberarArbol(NodoBST* nodo) {
        if (nodo == nullptr) return;
        liberarArbol(nodo->izquierdo);
        liberarArbol(nodo->derecho);
        delete nodo;
    }

    NodoBST* insertarAuxiliar(NodoBST* nodo, Estudiante estudianteAInsertar) {
        if (nodo == nullptr) {
            return new NodoBST(estudianteAInsertar);
        }

        if (estudianteAInsertar.codigo < nodo->dato.codigo) {
            nodo->izquierdo = insertarAuxiliar(nodo->izquierdo, estudianteAInsertar);
        }
        else if (estudianteAInsertar.codigo > nodo->dato.codigo) {
            nodo->derecho = insertarAuxiliar(nodo->derecho, estudianteAInsertar);
        }
        else {
            throw runtime_error("Error: El codigo universitario ya existe en el BST.");
        }
        return nodo;
    }

    NodoBST* eliminarAuxiliar(NodoBST* nodo, int codigoAEliminar) {
        if (nodo == nullptr) {
            return nullptr;
        }

        if (codigoAEliminar < nodo->dato.codigo) {
            nodo->izquierdo = eliminarAuxiliar(nodo->izquierdo, codigoAEliminar);
        }
        else if (codigoAEliminar > nodo->dato.codigo) {
            nodo->derecho = eliminarAuxiliar(nodo->derecho, codigoAEliminar);
        }
        else {
            if (nodo->izquierdo == nullptr) {
                NodoBST* temporal = nodo->derecho;
                delete nodo;
                return temporal;
            }
            if (nodo->derecho == nullptr) {
                NodoBST* temporal = nodo->izquierdo;
                delete nodo;
                return temporal;
            }

            NodoBST* sucesor = buscarMinimo(nodo->derecho);
            nodo->dato = sucesor->dato;
            nodo->derecho = eliminarAuxiliar(nodo->derecho, sucesor->dato.codigo);
        }
        return nodo;
    }

    const NodoBST* buscarAuxiliar(const NodoBST* nodo, int codigoABuscar) const {
        if (nodo == nullptr || nodo->dato.codigo == codigoABuscar) {
            return nodo;
        }
        if (codigoABuscar < nodo->dato.codigo) {
            return buscarAuxiliar(nodo->izquierdo, codigoABuscar);
        }
        return buscarAuxiliar(nodo->derecho, codigoABuscar);
    }

    NodoBST* buscarMinimo(NodoBST* nodo) const {
        while (nodo->izquierdo != nullptr) {
            nodo = nodo->izquierdo;
        }
        return nodo;
    }

    int alturaAuxiliar(const NodoBST* nodo) const {
        if (nodo == nullptr) {
            return -1;
        }
        int alturaIzquierda = alturaAuxiliar(nodo->izquierdo);
        int alturaDerecha = alturaAuxiliar(nodo->derecho);
        return 1 + max(alturaIzquierda, alturaDerecha);
    }

    void inOrderAuxiliar(const NodoBST* nodo, vector<Estudiante>& listaResultado) const {
        if (nodo == nullptr) return;
        inOrderAuxiliar(nodo->izquierdo, listaResultado);
        listaResultado.push_back(nodo->dato);
        inOrderAuxiliar(nodo->derecho, listaResultado);
    }

    void preOrderAuxiliar(const NodoBST* nodo, vector<Estudiante>& listaResultado) const {
        if (nodo == nullptr) return;
        listaResultado.push_back(nodo->dato);
        preOrderAuxiliar(nodo->izquierdo, listaResultado);
        preOrderAuxiliar(nodo->derecho, listaResultado);
    }

    void postOrderAuxiliar(const NodoBST* nodo, vector<Estudiante>& listaResultado) const {
        if (nodo == nullptr) return;
        postOrderAuxiliar(nodo->izquierdo, listaResultado);
        postOrderAuxiliar(nodo->derecho, listaResultado);
        listaResultado.push_back(nodo->dato);
    }
};

} // namespace una_puno

using namespace una_puno;

void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void ejecutarBenchmark() {
    cout << "\n--- Ejecutando Benchmark Experimental (Dataset a Gran Escala) ---\n";
    ArbolAcademico arbolPruebas;
    const int TAMANO_DATASET = 50000;

    mt19937 generadorAleatorio(12345);
    uniform_int_distribution<int> distCodigo(10000000, 29999999);

    vector<int> codigosInsertados;
    codigosInsertados.reserve(TAMANO_DATASET);

    auto inicio = chrono::high_resolution_clock::now();
    for(int i = 0; i < TAMANO_DATASET; ++i) {
        int cod = distCodigo(generadorAleatorio);
        try {
            arbolPruebas.insertar(Estudiante(cod, "Estudiante Benchmark", "Escuela Sede", 14.0f, 100, EstadoAcademico::ACTIVO, "2026-I"));
            codigosInsertados.push_back(cod);
        } catch(...) {
            i--;
        }
    }
    auto fin = chrono::high_resolution_clock::now();
    double tiempoInsercion = chrono::duration_cast<chrono::microseconds>(fin - inicio).count() / 1000.0;

    inicio = chrono::high_resolution_clock::now();
    for(size_t i = 0; i < codigosInsertados.size(); ++i) {
        arbolPruebas.buscar(codigosInsertados[i]);
    }
    fin = chrono::high_resolution_clock::now();
    double tiempoBusqueda = chrono::duration_cast<chrono::microseconds>(fin - inicio).count() / 1000.0;

    cout << "Resultados de rendimiento optimizados (Punteros Clasicos):\n";
    cout << " -> Tamano del Dataset : " << TAMANO_DATASET << " registros\n";
    cout << " -> Tiempo de Insercion: " << tiempoInsercion << " ms\n";
    cout << " -> Tiempo de Busqueda : " << tiempoBusqueda << " ms\n";
}

int main() {
    ArbolAcademico arbol;

    arbol.insertar(Estudiante(20210500, "Mamani Quispe, Juan", "Ing. Sistemas", 15.8f, 120, EstadoAcademico::ACTIVO, "2021-I"));
    arbol.insertar(Estudiante(20210300, "Huanca Apaza, Maria", "Ing. Civil", 14.2f, 110, EstadoAcademico::ACTIVO, "2021-I"));
    arbol.insertar(Estudiante(20210700, "Condori Flores, Pedro", "Medicina", 17.1f, 130, EstadoAcademico::ACTIVO, "2021-I"));
    arbol.insertar(Estudiante(20210100, "Ticona Lupaca, Rosa", "Contabilidad", 12.0f, 90, EstadoAcademico::SUSPENDIDO, "2021-I"));
    arbol.insertar(Estudiante(20210400, "Larico Ccama, Carlos", "Ing. Sistemas", 16.5f, 115, EstadoAcademico::ACTIVO, "2021-I"));
    arbol.insertar(Estudiante(20210600, "Cutipa Vargas, Elena", "Agronomia", 13.7f, 100, EstadoAcademico::ACTIVO, "2021-I"));
    arbol.insertar(Estudiante(20210900, "Pari Choque, Luis", "Ing. Sistemas", 18.3f, 140, EstadoAcademico::EGRESADO, "2021-I"));

    // --- EJECUCIÓN OBLIGATORIA DE LAS VERIFICACIONES P3 Y P4 ---
    assert(arbol.contarNodos(arbol.obtenerRaiz()) == (int)arbol.obtenerInOrder().size());
    assert(arbol.esArbolBST(arbol.obtenerRaiz()));
    std::cout << "P3 y P4 VERIFICADAS \xE2\x9C\x93\n"; // Hexadecimal para evitar problemas con la codificación de la consola en Windows
    // ------------------------------------------------------------

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
        cout << "7. Ejecutar Benchmark de Rendimiento \n";
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
                    arbol.insertar(Estudiante(cod, nom, esc, ppa, cred, est, sem));
                    cout << ">> Estudiante registrado con exito.\n";

                    // Re-verificar tras mutar la estructura
                    assert(arbol.contarNodos(arbol.obtenerRaiz()) == (int)arbol.obtenerInOrder().size());
                    assert(arbol.esArbolBST(arbol.obtenerRaiz()));
                } catch (const exception& e) {
                    cout << ">> Error al insertar: " << e.what() << '\n';
                }
                break;
            }
            case 2: {
                int cod;
                cout << "\nIngrese el codigo a buscar: "; cin >> cod;

                const Estudiante* resultado = arbol.buscar(cod);
                if (resultado != nullptr) {
                    cout << ">> Estudiante Encontrado:\n";
                    resultado->mostrarInformacion();
                } else {
                    cout << ">> Codigo " << cod << " no existe en el sistema.\n";
                }
                break;
            }
            case 3: {
                int cod;
                cout << "\nIngrese el codigo a eliminar: "; cin >> cod;
                try {
                    arbol.eliminar(cod);
                    cout << ">> Registro eliminado correctamente del BST.\n";

                    // Re-verificar tras mutar la estructura
                    assert(arbol.contarNodos(arbol.obtenerRaiz()) == (int)arbol.obtenerInOrder().size());
                    assert(arbol.esArbolBST(arbol.obtenerRaiz()));
                } catch (const exception& e) {
                    cout << ">> Error: " << e.what() << '\n';
                }
                break;
            }
            case 4: {
                cout << "\n--- Estudiantes Registrados (Orden Ascendente In-Order) ---\n";
                vector<Estudiante> lista = arbol.obtenerInOrder();
                if (lista.empty()) {
                    cout << "El arbol esta vacio.\n";
                } else {
                    for (size_t i = 0; i < lista.size(); ++i) {
                        lista[i].mostrarInformacion();
                    }
                }
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
                    vector<Estudiante> aux = arbol.obtenerPreOrder();
                    for (size_t i = 0; i < aux.size(); ++i) cout << aux[i].codigo << " ";
                    cout << "\n";
                } else if (tipoRecorrido == 2) {
                    cout << "\n[Recorrido Post-Order]\n";
                    vector<Estudiante> aux = arbol.obtenerPostOrder();
                    for (size_t i = 0; i < aux.size(); ++i) cout << aux[i].codigo << " ";
                    cout << "\n";
                } else if (tipoRecorrido == 3) {
                    cout << "\n[Recorrido por Niveles / BFS]\n";
                    vector<Estudiante> aux = arbol.obtenerBFS();
                    for (size_t i = 0; i < aux.size(); ++i) cout << aux[i].codigo << " ";
                    cout << "\n";
                } else {
                    cout << "Modo no valido.\n";
                }
                break;
            }
            case 6:
                arbol.mostrarEstadisticas();
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

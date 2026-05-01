// ================================================================
// PRÁCTICA: TABLAS HASH EN C++ 
// ================================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// ───────────────────────────────────────────────────────────────
// ESTRUCTURA
// ───────────────────────────────────────────────────────────────
struct Registro {
    string clave;
    string valor;
};

// ───────────────────────────────────────────────────────────────
// FUNCIÓN HASH (polinomial)
// ───────────────────────────────────────────────────────────────
size_t funcionHash(const string& clave, size_t tamanio) {
    size_t hash = 0;
    for (char c : clave) {
        hash = (hash * 31 + static_cast<size_t>(c)) % tamanio;
    }
    return hash;
}

// ───────────────────────────────────────────────────────────────
// TABLA HASH: ENCADENAMIENTO
// ───────────────────────────────────────────────────────────────
class TablaHashEncadenamiento {
public:
    size_t tamanio;
    vector<list<Registro>> tabla;
    long long colisiones = 0;
    long long elementos = 0;

    TablaHashEncadenamiento(size_t m) : tamanio(m), tabla(m) {}

    void insertar(const Registro& reg) {
        size_t idx = funcionHash(reg.clave, tamanio);

        if (!tabla[idx].empty()) colisiones++;

        tabla[idx].push_back(reg);
        elementos++;
    }

    Registro* buscar(const string& clave) {
        size_t idx = funcionHash(clave, tamanio);

        for (auto& reg : tabla[idx]) {
            if (reg.clave == clave)
                return &reg;
        }
        return nullptr;
    }

    double factorDeCarga() const {
        return (double)elementos / tamanio;
    }
};
// ───────────────────────────────────────────────────────────────
// TABLA HASH: SONDEO LINEAL (MEJORADA)
// ───────────────────────────────────────────────────────────────
class TablaHashSondeoLineal {
    enum Estado { VACIO, OCUPADO, ELIMINADO };
public:
    size_t tamanio;
    vector<Registro> tabla;
    vector<Estado> estados;
    long long colisiones = 0;
    long long elementos = 0;

    TablaHashSondeoLineal(size_t m)
        : tamanio(m), tabla(m), estados(m, VACIO) {}

    bool insertar(const Registro& reg) {

        // Control de factor de carga
        if ((double)elementos / tamanio >= 0.75)
            return false;

        size_t idx = funcionHash(reg.clave, tamanio);
        size_t pasos = 0;

        int primerEliminado = -1;

        while (estados[idx] != VACIO) {

            if (estados[idx] == OCUPADO) {
                colisiones++;
          }
            // Guardar primer eliminado para reutilizarlo
            if (estados[idx] == ELIMINADO && primerEliminado == -1) {
                primerEliminado = idx;
            }

            idx = (idx + 1) % tamanio;

            if (++pasos >= tamanio)
                return false;
        }

        // Si encontramos un eliminado antes, usamos ese
        if (primerEliminado != -1) {
            idx = primerEliminado;
        }

        tabla[idx] = reg;
        estados[idx] = OCUPADO;
        elementos++;

        return true;
    }

    Registro* buscar(const string& clave) {
        size_t idx = funcionHash(clave, tamanio);
        size_t pasos = 0;

        while (estados[idx] != VACIO) {

            if (estados[idx] == OCUPADO && tabla[idx].clave == clave) {
                return &tabla[idx];
            }

            idx = (idx + 1) % tamanio;

            if (++pasos >= tamanio)
                break;
        }

        return nullptr;
    }
};
// ───────────────────────────────────────────────────────────────
// LECTURA CSV (simple)
// ───────────────────────────────────────────────────────────────
vector<Registro> leerCSV(const string& ruta, int col_clave, int col_valor) {
    vector<Registro> registros;
    ifstream archivo(ruta);

    if (!archivo.is_open()) {
        cerr << "Error: no se pudo abrir " << ruta << endl;
        return registros;
    }

    string linea;
    getline(archivo, linea); // encabezado
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string campo;
        vector<string> campos;

        while (getline(ss, campo, ',')) {
            campos.push_back(campo);
        }

        if ((int)campos.size() > max(col_clave, col_valor)) {
            Registro r;
            r.clave = campos[col_clave];
            r.valor = campos[col_valor];

            if (!r.clave.empty()) {
                registros.push_back(r);
            }
        }
    }

    archivo.close();
    return registros;
}
//───────────────────────────────────────────────────────────────
// BENCHMARK
// ───────────────────────────────────────────────────────────────
void benchmark(const vector<Registro>& datos, vector<size_t> tamanios) {
    cout << left
         << setw(8) << "m"
         << setw(8) << "n"
         << setw(12) << "Col_Enc"
         << setw(15) << "Ins_Enc(us)"
         << setw(12) << "Col_SL"
         << setw(15) << "Ins_SL(us)"
         << endl;
    cout << string(70, '-') << endl;
    for (size_t m : tamanios) {

        size_t n = min(datos.size(), (size_t)(m * 0.65));

        // Encadenamiento
        TablaHashEncadenamiento th_enc(m);

        auto t0 = high_resolution_clock::now();
        for (size_t i = 0; i < n; i++)
            th_enc.insertar(datos[i]);
        auto t1 = high_resolution_clock::now();

        auto dur_enc = duration_cast<microseconds>(t1 - t0).count();

        // Sondeo lineal
        TablaHashSondeoLineal th_sl(m);

        t0 = high_resolution_clock::now();
        for (size_t i = 0; i < n; i++)
            th_sl.insertar(datos[i]);
        t1 = high_resolution_clock::now();

        auto dur_sl = duration_cast<microseconds>(t1 - t0).count();

        cout << left
             << setw(8) << m
             << setw(8) << n
             << setw(12) << th_enc.colisiones
             << setw(15) << dur_enc
             << setw(12) << th_sl.colisiones
             << setw(15) << dur_sl
             << endl;
    }
}

// ───────────────────────────────────────────────────────────────
// MAIN
// ───────────────────────────────────────────────────────────────
int main() {

    cout << "=== BENCHMARK TABLAS HASH ===\n";

    vector<Registro> datos;

    for (int i = 0; i < 5000; i++) {
        Registro r;
        r.clave = "user_" + to_string(i + 1);
        r.valor = "registro_" + to_string(i);
        datos.push_back(r);
    }

    cout << "Registros generados: " << datos.size() << "\n\n";

    vector<size_t> tamanios = {1009, 2003, 4001, 8009};

    benchmark(datos, tamanios);

    return 0;
}

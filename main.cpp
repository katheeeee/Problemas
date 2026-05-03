#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <chrono>
#include <string>

using namespace std;

// =========================
// ESTRUCTURA DE REGISTRO
// =========================
struct Record {
    string key;
    string value;
};

// =========================
// HASH CON ENCADENAMIENTO
// =========================
class HashTableChaining {
private:
    int size;
    vector<list<Record>> table;
    int collisions;

public:
    HashTableChaining(int tableSize) {
        size = tableSize;
        table.resize(size);
        collisions = 0;
    }

    int hashFunction(string key) {
        unsigned long hash = 0;
        for (char c : key) {
            hash = hash * 31 + c;
        }
        return hash % size;
    }

    void insert(string key, string value) {
        int index = hashFunction(key);

        if (!table[index].empty()) {
            collisions++;
        }

        table[index].push_back({key, value});
    }

    string search(string key) {
        int index = hashFunction(key);

        for (const Record& r : table[index]) {
            if (r.key == key) {
                return r.value;
            }
        }

        return "No encontrado";
    }

    int getCollisions() {
        return collisions;
    }

    double loadFactor(int totalElements) {
        return static_cast<double>(totalElements) / size;
    }
};

// =========================
// HASH CON SONDEO LINEAL
// =========================
class HashTableLinear {
private:
    int size;
    vector<Record> table;
    vector<bool> occupied;
    int collisions;

public:
    HashTableLinear(int tableSize) {
        size = tableSize;
        table.resize(size);
        occupied.resize(size, false);
        collisions = 0;
    }

    int hashFunction(string key) {
        unsigned long hash = 0;
        for (char c : key) {
            hash = hash * 31 + c;
        }
        return hash % size;
    }

    void insert(string key, string value) {
        int index = hashFunction(key);
        int start = index;

        while (occupied[index]) {
            collisions++;
            index = (index + 1) % size;

            if (index == start) {
                cout << "Tabla llena\n";
                return;
            }
        }

        table[index] = {key, value};
        occupied[index] = true;
    }

    string search(string key) {
        int index = hashFunction(key);
        int start = index;

        while (occupied[index]) {
            if (table[index].key == key) {
                return table[index].value;
            }

            index = (index + 1) % size;

            if (index == start) break;
        }

        return "No encontrado";
    }

    int getCollisions() {
        return collisions;
    }

    double loadFactor(int totalElements) {
        return static_cast<double>(totalElements) / size;
    }
};

// =========================
// LECTURA DE CSV
// =========================
vector<string> readKeysFromCSV(string filename, int limit) {
    vector<string> keys;
    ifstream file(filename);

    cout << "Intentando abrir: " << filename << endl;

    if (!file.is_open()) {
        cout << "ERROR: No se pudo abrir el archivo\n";
        return keys;
    }

    string line;

    // saltar cabecera
    getline(file, line);

    while (getline(file, line) && keys.size() < limit) {
        stringstream ss(line);
        string column;

        // leer primera columna
        getline(ss, column, ',');

        if (!column.empty()) {
            keys.push_back(column);
        }
    }

    cout << "Claves cargadas: " << keys.size() << endl;

    file.close();
    return keys;
}

// =========================
// MAIN
// =========================
int main() {

    // 🔴 IMPORTANTE: SI FALLA, USA RUTA COMPLETA
    string filename = "ecommerce_sample.csv";

    // Ejemplo de ruta completa (descomenta si necesitas)
    //7string filename = "C:\\Users\\TUF GAMING\\OneDrive\\Documentos\\2026-\\CodigosPy\\TblaHash5-6\\bin\\Debug\\ecommerce_sample.csv";

    int limit = 10000;
    int tableSize = 20011;

    vector<string> keys = readKeysFromCSV(filename, limit);

    if (keys.empty()) {
        cout << "No se cargaron datos.\n";
        return 0;
    }

    HashTableChaining hashChain(tableSize);
    HashTableLinear hashLinear(tableSize);

    // =========================
    // ENCADENAMIENTO
    // =========================
    auto start = chrono::high_resolution_clock::now();

    for (string key : keys) {
        hashChain.insert(key, key);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> insertChainTime = end - start;

    start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000 && i < keys.size(); i++) {
        hashChain.search(keys[i]);
    }

    end = chrono::high_resolution_clock::now();
    chrono::duration<double> searchChainTime = end - start;

    // =========================
    // SONDEO LINEAL
    // =========================
    start = chrono::high_resolution_clock::now();

    for (string key : keys) {
        hashLinear.insert(key, key);
    }

    end = chrono::high_resolution_clock::now();
    chrono::duration<double> insertLinearTime = end - start;

    start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000 && i < keys.size(); i++) {
        hashLinear.search(keys[i]);
    }

    end = chrono::high_resolution_clock::now();
    chrono::duration<double> searchLinearTime = end - start;

    // =========================
    // RESULTADOS
    // =========================
    cout << "\nRESULTADOS DEL EXPERIMENTO\n";
    cout << "---------------------------\n";

    cout << "\nEncadenamiento separado:\n";
    cout << "Tiempo de insercion: " << insertChainTime.count() << " s\n";
    cout << "Tiempo de busqueda: " << searchChainTime.count() << " s\n";
    cout << "Colisiones: " << hashChain.getCollisions() << "\n";
    cout << "Factor de carga: " << hashChain.loadFactor(keys.size()) << "\n";

    cout << "\nSondeo lineal:\n";
    cout << "Tiempo de insercion: " << insertLinearTime.count() << " s\n";
    cout << "Tiempo de busqueda: " << searchLinearTime.count() << " s\n";
    cout << "Colisiones: " << hashLinear.getCollisions() << "\n";
    cout << "Factor de carga: " << hashLinear.loadFactor(keys.size()) << "\n";

    return 0;
}

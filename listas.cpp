#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

vector<int> leerVarios() {
    vector<int> valores;
    string linea;
    cin.ignore();
    getline(cin, linea);
    stringstream ss(linea);
    int num;
    while (ss >> num) {
        valores.push_back(num);
    }
    return valores;
}

template<typename T>
class Pila {
private:
    struct Nodo {
        T dato;
        Nodo* siguiente;
        Nodo(const T& valor) : dato(valor), siguiente(nullptr) {}
    };
    Nodo* tope;
public:
    Pila() : tope(nullptr) {}

    void push(const T& valor) {
        Nodo* nuevo = new Nodo(valor);
        nuevo->siguiente = tope;
        tope = nuevo;
        cout << "Insertado: " << valor << endl;
    }

    T pop() {
        if (!tope) throw runtime_error("Pila vacia");
        Nodo* aux = tope;
        T valor = aux->dato;
        tope = tope->siguiente;
        delete aux;
        cout << "Eliminado: " << valor << endl;
        return valor;
    }

    T peek() {
        if (!tope) throw runtime_error("Pila vacia");
        return tope->dato;
    }

    void recorrer() {
        if (!tope) {
            cout << "Pila vacia" << endl;
            return;
        }
        Nodo* actual = tope;
        cout << "Pila (tope -> fondo): ";
        while (actual) {
            cout << actual->dato << " ";
            actual = actual->siguiente;
        }
        cout << endl;
    }

    ~Pila() { while (tope) pop(); }
};

template<typename T>
class Cola {
private:
    struct Nodo {
        T dato;
        Nodo* siguiente;
        Nodo(const T& valor) : dato(valor), siguiente(nullptr) {}
    };
    Nodo* frente;
    Nodo* final;
public:
    Cola() : frente(nullptr), final(nullptr) {}

    void enqueue(const T& valor) {
        Nodo* nuevo = new Nodo(valor);
        if (!final) {
            frente = final = nuevo;
        } else {
            final->siguiente = nuevo;
            final = nuevo;
        }
        cout << "Insertado: " << valor << endl;
    }

    T dequeue() {
        if (!frente) throw runtime_error("Cola vacia");
        Nodo* aux = frente;
        T valor = aux->dato;
        frente = frente->siguiente;
        if (!frente) final = nullptr;
        delete aux;
        cout << "Eliminado: " << valor << endl;
        return valor;
    }

    T peek() {
        if (!frente) throw runtime_error("Cola vacia");
        return frente->dato;
    }

    void recorrer() {
        if (!frente) {
            cout << "Cola vacia" << endl;
            return;
        }
        Nodo* actual = frente;
        cout << "Cola (frente -> final): ";
        while (actual) {
            cout << actual->dato << " ";
            actual = actual->siguiente;
        }
        cout << endl;
    }

    ~Cola() { while (frente) dequeue(); }
};

template<typename T>
class ListaEnlazada {
private:
    struct Nodo {
        T dato;
        Nodo* siguiente;
        Nodo(const T& valor) : dato(valor), siguiente(nullptr) {}
    };
    Nodo* cabeza;
public:
    ListaEnlazada() : cabeza(nullptr) {}

    void insertarInicio(const T& valor) {
        Nodo* nuevo = new Nodo(valor);
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        cout << "Insertado al inicio: " << valor << endl;
    }

    void insertarFinal(const T& valor) {
        Nodo* nuevo = new Nodo(valor);
        if (!cabeza) {
            cabeza = nuevo;
        } else {
            Nodo* actual = cabeza;
            while (actual->siguiente) actual = actual->siguiente;
            actual->siguiente = nuevo;
        }
        cout << "Insertado al final: " << valor << endl;
    }

    T eliminarInicio() {
        if (!cabeza) throw runtime_error("Lista vacia");
        Nodo* aux = cabeza;
        T valor = aux->dato;
        cabeza = cabeza->siguiente;
        delete aux;
        cout << "Eliminado del inicio: " << valor << endl;
        return valor;
    }

    void mostrar() {
        if (!cabeza) {
            cout << "Lista vacia" << endl;
            return;
        }
        Nodo* actual = cabeza;
        cout << "Lista: ";
        while (actual) {
            cout << actual->dato << " -> ";
            actual = actual->siguiente;
        }
        cout << "NULL" << endl;
    }

    ~ListaEnlazada() { while (cabeza) eliminarInicio(); }
};

void menuPila() {
    Pila<int> pila;
    int opcion;
    string input;

    do {
        cout << "\n--- PILA ---" << endl;
        cout << "1. Push" << endl;
        cout << "2. Pop" << endl;
        cout << "3. Ver tope" << endl;
        cout << "4. Recorrer" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        switch(opcion) {
            case 1:
                cout << "Ingrese valores (separados por espacio): ";
                {
                    vector<int> vals = leerVarios();
                    for (int v : vals) {
                        pila.push(v);
                    }
                }
                break;
            case 2:
                try {
                    pila.pop();
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            case 3:
                try {
                    cout << "Tope: " << pila.peek() << endl;
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            case 4:
                pila.recorrer();
                break;
        }
    } while (opcion != 0);
}

void menuCola() {
    Cola<int> cola;
    int opcion;

    do {
        cout << "\n--- COLA ---" << endl;
        cout << "1. Enqueue" << endl;
        cout << "2. Dequeue" << endl;
        cout << "3. Ver frente" << endl;
        cout << "4. Recorrer" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        switch(opcion) {
            case 1:
                cout << "Ingrese valores (separados por espacio): ";
                {
                    vector<int> vals = leerVarios();
                    for (int v : vals) {
                        cola.enqueue(v);
                    }
                }
                break;
            case 2:
                try {
                    cola.dequeue();
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            case 3:
                try {
                    cout << "Frente: " << cola.peek() << endl;
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            case 4:
                cola.recorrer();
                break;
        }
    } while (opcion != 0);
}

void menuLista() {
    ListaEnlazada<int> lista;
    int opcion;

    do {
        cout << "\n--- LISTA ENLAZADA ---" << endl;
        cout << "1. Insertar al inicio" << endl;
        cout << "2. Insertar al final" << endl;
        cout << "3. Eliminar del inicio" << endl;
        cout << "4. Mostrar" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        switch(opcion) {
            case 1:
                cout << "Ingrese valores (separados por espacio): ";
                {
                    vector<int> vals = leerVarios();
                    for (int v : vals) {
                        lista.insertarInicio(v);
                    }
                }
                break;
            case 2:
                cout << "Ingrese valores (separados por espacio): ";
                {
                    vector<int> vals = leerVarios();
                    for (int v : vals) {
                        lista.insertarFinal(v);
                    }
                }
                break;
            case 3:
                try {
                    lista.eliminarInicio();
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            case 4:
                lista.mostrar();
                break;
        }
    } while (opcion != 0);
}

int main() {
    int opcion;
    do {
        cout << "\n--- MENU PRINCIPAL ---" << endl;
        cout << "1. Pila" << endl;
        cout << "2. Cola" << endl;
        cout << "3. Lista Enlazada" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;
        switch(opcion) {
            case 1: menuPila(); break;
            case 2: menuCola(); break;
            case 3: menuLista(); break;
        }
    } while (opcion != 0);
    return 0;
}

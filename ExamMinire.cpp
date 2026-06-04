#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <stack>
#include <queue>

using namespace std;

struct Estudiante
{
    string codigo;
    string nombre;
    float nota1;
    float nota2;
    float nota3;
    float promedio;
};

vector<Estudiante> hoja;
unordered_map<string, int> indiceHash;
stack<vector<Estudiante>> historial;
queue<string> colaAtencion;

float calcularPromedio(float n1, float n2, float n3)
{
    return (n1 + n2 + n3) / 3.0f;
}

void actualizarHash()
{
    indiceHash.clear();

    for (int i = 0; i < (int)hoja.size(); i++)
    {
        indiceHash[hoja[i].codigo] = i;
    }
}

void guardarHistorial()
{
    historial.push(hoja);
}

void registrarEstudiante()
{
    Estudiante e;

    cout << "\nCodigo: ";
    cin >> e.codigo;

    if (indiceHash.count(e.codigo))
    {
        cout << "Codigo ya registrado.\n";
        return;
    }

    cin.ignore();

    cout << "Nombre: ";
    getline(cin, e.nombre);

    cout << "Nota 1: ";
    cin >> e.nota1;

    cout << "Nota 2: ";
    cin >> e.nota2;

    cout << "Nota 3: ";
    cin >> e.nota3;

    e.promedio = calcularPromedio(e.nota1, e.nota2, e.nota3);

    guardarHistorial();

    hoja.push_back(e);

    actualizarHash();

    cout << "Estudiante registrado.\n";
}

void mostrarHoja()
{
    if (hoja.empty())
    {
        cout << "\nNo hay registros.\n";
        return;
    }

    cout << "\n";
    cout << left
         << setw(12) << "Codigo"
         << setw(25) << "Nombre"
         << setw(8) << "N1"
         << setw(8) << "N2"
         << setw(8) << "N3"
         << setw(10) << "Promedio"
         << endl;

    for (const auto &e : hoja)
    {
        cout << left
             << setw(12) << e.codigo
             << setw(25) << e.nombre
             << setw(8) << e.nota1
             << setw(8) << e.nota2
             << setw(8) << e.nota3
             << setw(10) << fixed << setprecision(2)
             << e.promedio
             << endl;
    }
}

void buscarEstudiante()
{
    string codigo;

    cout << "\nCodigo a buscar: ";
    cin >> codigo;

    auto it = indiceHash.find(codigo);

    if (it == indiceHash.end())
    {
        cout << "No encontrado.\n";
        return;
    }

    Estudiante &e = hoja[it->second];

    cout << "\nNombre: " << e.nombre;
    cout << "\nPromedio: " << e.promedio << endl;
}

int particion(vector<Estudiante> &arr, int bajo, int alto)
{
    float pivote = arr[alto].promedio;

    int i = bajo - 1;

    for (int j = bajo; j < alto; j++)
    {
        if (arr[j].promedio >= pivote)
        {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[alto]);

    return i + 1;
}

void quickSort(vector<Estudiante> &arr, int bajo, int alto)
{
    if (bajo < alto)
    {
        int pi = particion(arr, bajo, alto);

        quickSort(arr, bajo, pi - 1);
        quickSort(arr, pi + 1, alto);
    }
}

void merge(vector<Estudiante> &arr, int izq, int medio, int der)
{
    int n1 = medio - izq + 1;
    int n2 = der - medio;

    vector<Estudiante> L(n1);
    vector<Estudiante> R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[izq + i];

    for (int j = 0; j < n2; j++)
        R[j] = arr[medio + 1 + j];

    int i = 0;
    int j = 0;
    int k = izq;

    while (i < n1 && j < n2)
    {
        if (L[i].codigo <= R[j].codigo)
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];

    while (j < n2)
        arr[k++] = R[j++];
}

void mergeSort(vector<Estudiante> &arr, int izq, int der)
{
    if (izq < der)
    {
        int medio = (izq + der) / 2;

        mergeSort(arr, izq, medio);
        mergeSort(arr, medio + 1, der);

        merge(arr, izq, medio, der);
    }
}

void ordenarPorPromedio()
{
    if (hoja.empty())
        return;

    guardarHistorial();

    quickSort(hoja, 0, hoja.size() - 1);

    actualizarHash();

    cout << "Ordenado por promedio.\n";
}

void ordenarPorCodigo()
{
    if (hoja.empty())
        return;

    guardarHistorial();

    mergeSort(hoja, 0, hoja.size() - 1);

    actualizarHash();

    cout << "Ordenado por codigo.\n";
}

void deshacer()
{
    if (historial.empty())
    {
        cout << "Nada que deshacer.\n";
        return;
    }

    hoja = historial.top();
    historial.pop();

    actualizarHash();

    cout << "Operacion deshecha.\n";
}

void agregarCola()
{
    string codigo;

    cout << "\nCodigo: ";
    cin >> codigo;

    if (!indiceHash.count(codigo))
    {
        cout << "No existe.\n";
        return;
    }

    colaAtencion.push(codigo);

    cout << "Agregado a la cola.\n";
}

void atenderCola()
{
    if (colaAtencion.empty())
    {
        cout << "Cola vacia.\n";
        return;
    }

    string codigo = colaAtencion.front();
    colaAtencion.pop();

    cout << "Atendiendo: "
         << hoja[indiceHash[codigo]].nombre
         << endl;
}

float sumaRecursiva(int i)
{
    if (i >= (int)hoja.size())
        return 0;

    return hoja[i].promedio + sumaRecursiva(i + 1);
}

void estadisticas()
{
    if (hoja.empty())
    {
        cout << "Sin datos.\n";
        return;
    }

    float suma = sumaRecursiva(0);

    cout << "\nCantidad: "
         << hoja.size();

    cout << "\nPromedio General: "
         << fixed << setprecision(2)
         << suma / hoja.size()
         << endl;
}

void menu()
{
    int opcion;

    do
    {
        cout << "\n==============================\n";
        cout << "1. Registrar estudiante\n";
        cout << "2. Mostrar hoja\n";
        cout << "3. Buscar estudiante\n";
        cout << "4. Ordenar por promedio\n";
        cout << "5. Ordenar por codigo\n";
        cout << "6. Deshacer\n";
        cout << "7. Agregar a cola\n";
        cout << "8. Atender cola\n";
        cout << "9. Estadisticas\n";
        cout << "10. Salir\n";
        cout << "Opcion: ";

        cin >> opcion;

        switch (opcion)
        {
        case 1: registrarEstudiante(); break;
        case 2: mostrarHoja(); break;
        case 3: buscarEstudiante(); break;
        case 4: ordenarPorPromedio(); break;
        case 5: ordenarPorCodigo(); break;
        case 6: deshacer(); break;
        case 7: agregarCola(); break;
        case 8: atenderCola(); break;
        case 9: estadisticas(); break;
        case 10: cout << "Fin del programa.\n"; break;
        default: cout << "Opcion invalida.\n";
        }

    } while (opcion != 10);
}

int main()
{
    menu();
    return 0;
}

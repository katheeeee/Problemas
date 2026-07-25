#include "rbt.hpp"
#include <iostream>
#include <limits>

void mostrarVotante(NodoRBT* n) {
    if (!n) {
        std::cout << "No se encontro un votante con ese DNI.\n";
        return;
    }
    std::cout << "DNI: " << n->votante.dni
              << " | Nombre: " << n->votante.nombre
              << " | Facultad: " << n->votante.facultad
              << " | Habilitado: " << (n->votante.habilitado ? "Si" : "No") << "\n";
}

void limpiarEntrada() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    ArbolElectoral arbol;
    int opcion = -1;

    while (opcion != 0) {
        std::cout << "\n==== Sistema de Registro Electoral (RBT) ====\n";
        std::cout << "1. Registrar votante\n";
        std::cout << "2. Buscar votante por DNI\n";
        std::cout << "3. Eliminar votante por DNI\n";
        std::cout << "4. Verificar validez del arbol (propiedades RBT)\n";
        std::cout << "5. Mostrar altura del arbol\n";
        std::cout << "0. Salir\n";
        std::cout << "Opcion: ";

        if (!(std::cin >> opcion)) {
            limpiarEntrada();
            continue;
        }

        if (opcion == 1) {
            Votante v;
            std::cout << "DNI: ";
            std::cin >> v.dni;
            std::cout << "Nombre: ";
            limpiarEntrada();
            std::getline(std::cin, v.nombre);
            std::cout << "Facultad: ";
            std::getline(std::cin, v.facultad);
            std::cout << "Habilitado (1 = si, 0 = no): ";
            int h;
            std::cin >> h;
            v.habilitado = (h != 0);
            arbol.insertar(v);
            std::cout << "Votante registrado.\n";
        } else if (opcion == 2) {
            std::string dni;
            std::cout << "DNI a buscar: ";
            std::cin >> dni;
            mostrarVotante(arbol.buscar(dni));
        } else if (opcion == 3) {
            std::string dni;
            std::cout << "DNI a eliminar: ";
            std::cin >> dni;
            if (arbol.eliminar(dni)) std::cout << "Votante eliminado.\n";
            else std::cout << "No se encontro un votante con ese DNI.\n";
        } else if (opcion == 4) {
            std::cout << (arbol.esRBTValido()
                              ? "El arbol cumple las propiedades de un RBT.\n"
                              : "El arbol NO cumple las propiedades de un RBT.\n");
        } else if (opcion == 5) {
            std::cout << "Altura del arbol: " << arbol.altura() << "\n";
        } else if (opcion != 0) {
            std::cout << "Opcion invalida.\n";
        }
    }

    std::cout << "Fin del programa.\n";
    return 0;
}

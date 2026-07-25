
#ifndef RBT_HPP
#define RBT_HPP

#include <iostream>
#include <string>
#include <vector>

enum class Color { ROJO, NEGRO };

//────────────────────────────────────────────────────────────
// Estructura de un votante
//────────────────────────────────────────────────────────────
struct Votante {
    std::string dni;
    std::string nombre;
    std::string facultad;
    bool habilitado;
};

//────────────────────────────────────────────────────────────
// Nodo del Árbol Rojo-Negro
//────────────────────────────────────────────────────────────
struct NodoRBT {
    Votante votante;
    Color color;

    NodoRBT* izquierdo;
    NodoRBT* derecho;
    NodoRBT* padre;

    NodoRBT();
};

//────────────────────────────────────────────────────────────
// Clase Árbol Electoral (Red-Black Tree)
//────────────────────────────────────────────────────────────
class ArbolElectoral {
private:
    NodoRBT* NIL;
    NodoRBT* raiz;

    std::vector<NodoRBT*> todos;

    // Rotaciones
    void rotIzq(NodoRBT* x);
    void rotDer(NodoRBT* y);

    // Inserción
    void fixInsertar(NodoRBT* z);

    // Eliminación
    NodoRBT* minimo(NodoRBT* x) const;
    void trasplantar(NodoRBT* u, NodoRBT* v);
    NodoRBT* buscarNodo(const std::string& dni) const;
    void fixEliminar(NodoRBT* x);

    // Verificación de propiedades
    bool p4(NodoRBT* n) const;
    int p5(NodoRBT* n) const;
    int altura(NodoRBT* n) const;

public:
    // Constructor y destructor
    ArbolElectoral();
    ~ArbolElectoral();

    // Operaciones principales
    void insertar(Votante v);
    bool eliminar(const std::string& dni);
    NodoRBT* buscar(const std::string& dni) const;

    // Verificaciones
    bool esRBTValido() const;
    int altura() const;
};

#endif

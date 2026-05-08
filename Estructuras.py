from collections import deque

#  PILA 
class Pila:
    def __init__(self):
        self.items = []
    
    def push(self, valor):
        self.items.append(valor)
        print(f"  Push: {valor}")
    
    def pop(self):
        if not self.items:
            raise IndexError("Pila vacia")
        return self.items.pop()
    
    def peek(self):
        if not self.items:
            raise IndexError("Pila vacia")
        return self.items[-1]
    
    def recorrer(self):
        print("  Pila (tope -> fondo):", self.items[::-1])

# COLA 
class Cola:
    def __init__(self):
        self.items = deque()
    
    def enqueue(self, valor):
        self.items.append(valor)
        print(f"  Enqueue: {valor}")
    
    def dequeue(self):
        if not self.items:
            raise IndexError("Cola vacia")
        return self.items.popleft()
    
    def peek(self):
        if not self.items:
            raise IndexError("Cola vacia")
        return self.items[0]
    
    def recorrer(self):
        print("  Cola (frente -> final):", list(self.items))

# LISTA ENLAZADA 
class Nodo:
    def __init__(self, valor):
        self.dato = valor
        self.siguiente = None

class ListaEnlazada:
    def __init__(self):
        self.cabeza = None
    
    def insertar_inicio(self, valor):
        nuevo = Nodo(valor)
        nuevo.siguiente = self.cabeza
        self.cabeza = nuevo
        print(f"  Insertar al inicio: {valor}")
    
    def insertar_final(self, valor):
        nuevo = Nodo(valor)
        if not self.cabeza:
            self.cabeza = nuevo
        else:
            actual = self.cabeza
            while actual.siguiente:
                actual = actual.siguiente
            actual.siguiente = nuevo
        print(f"  Insertar al final: {valor}")
    
    def eliminar_inicio(self):
        if not self.cabeza:
            raise IndexError("Lista vacia")
        valor = self.cabeza.dato
        self.cabeza = self.cabeza.siguiente
        print(f"  Eliminar del inicio: {valor}")
        return valor
    
    def mostrar(self):
        actual = self.cabeza
        print("  Lista: ", end="")
        while actual:
            print(f"{actual.dato} -> ", end="")
            actual = actual.siguiente
        print("NULL")

# ==================== EJECUCION PRINCIPAL ====================
if __name__ == "__main__":


    # ========== PARTE A: PILA ==========
    print("--- PILA (STACK) ---")
    pila = Pila()
    
    print("\nInsertar elementos: 5, 10, 15, 20, 25")
    for v in [5, 10, 15, 20, 25]:
        pila.push(v)
    pila.recorrer()
    
    print("\nEliminar dos elementos consecutivos")
    print(f"  Eliminado: {pila.pop()}")
    print(f"  Eliminado: {pila.pop()}")
    pila.recorrer()
    
    print("\nMostrar el tope")
    print(f"  Tope: {pila.peek()}")
    
    print("\n Estado final de la pila")
    pila.recorrer()
   
    print("\n" + "-"*40)
    print("\n--- COLA (QUEUE) ---")
    cola = Cola()
    
    print("\nInsertar elementos: 3, 6, 9, 12")
    for v in [3, 6, 9, 12]:
        cola.enqueue(v)
    cola.recorrer()
    
    print("\nEliminar un elemento")
    print(f"  Eliminado: {cola.dequeue()}")
    cola.recorrer()
    
    print("\nMostrar el frente")
    print(f"  Frente: {cola.peek()}")
    
    print("\nEstado final de la cola")
    cola.recorrer()
    
    print("\n" + "-"*40)
    print("\n--- LISTA ENLAZADA SIMPLE ---")
    lista = ListaEnlazada()
    
    print("\nInsertar al inicio: 8, 4")
    lista.insertar_inicio(8)
    lista.insertar_inicio(4)
    lista.mostrar()
    
    print("\nInsertar al final: 11")
    lista.insertar_final(11)
    lista.mostrar()
    
    print("\nEliminar el primer nodo")
    lista.eliminar_inicio()
    lista.mostrar()
    
    print("\nRecorrido completo de la lista")
    lista.mostrar()
    

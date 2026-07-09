class SparseMatrix:
    def __init__(self, rows, cols):
        self.rows = rows
        self.cols = cols
        self.data = {} # clave = (fila, col), valor = elemento

    def add(self, r, c, val):
        if val != 0:
            self.data[(r, c)] = val

    def get(self, r, c):
        return self.data.get((r, c), 0)

    def display(self):
        for r in range(self.rows):
            row = []
            for c in range(self.cols):
                row.append(str(self.get(r, c)))
            print(" ".join(row))

# ==========================================
# Ejemplo Base Ejecutable
# ==========================================
print("Matriz Dispersa (Formato COO):")
print("-" * 20)

A = SparseMatrix(4, 4)
A.add(0, 1, 5)
A.add(1, 3, 8)
A.add(2, 2, 3)

A.display()
print("-" * 20)

import pandas as pd
import time
import matplotlib.pyplot as plt


# =========================
# HASH CON ENCADENAMIENTO
# =========================
class HashTableChaining:
    def __init__(self, size):
        self.size = size
        self.table = [[] for _ in range(size)]
        self.collisions = 0

    def hash_function(self, key):
        return hash(key) % self.size

    def insert(self, key, value):
        index = self.hash_function(key)

        if len(self.table[index]) > 0:
            self.collisions += 1

        self.table[index].append((key, value))

    def search(self, key):
        index = self.hash_function(key)

        for k, v in self.table[index]:
            if k == key:
                return v

        return None

    def load_factor(self, total_elements):
        return total_elements / self.size


# =========================
# HASH CON SONDEO LINEAL
# =========================
class HashTableLinearProbing:
    def __init__(self, size):
        self.size = size
        self.table = [None] * size
        self.collisions = 0

    def hash_function(self, key):
        return hash(key) % self.size

    def insert(self, key, value):
        index = self.hash_function(key)
        start = index

        while self.table[index] is not None:
            self.collisions += 1
            index = (index + 1) % self.size

            if index == start:
                raise Exception("Tabla llena")

        self.table[index] = (key, value)

    def search(self, key):
        index = self.hash_function(key)
        start = index

        while self.table[index] is not None:
            if self.table[index][0] == key:
                return self.table[index][1]

            index = (index + 1) % self.size

            if index == start:
                break

        return None

    def load_factor(self, total_elements):
        return total_elements / self.size


# =========================
# HASH CON DOBLE HASHING (NUEVO)
# =========================
class HashTableDoubleHashing:
    def __init__(self, size):
        self.size = size
        self.table = [None] * size
        self.collisions = 0

    def hash1(self, key):
        return hash(key) % self.size

    def hash2(self, key):
        # Segunda funcion hash: debe ser impar y devolver un paso entre 1 y size-1
        return 1 + (hash(key) % (self.size - 1))

    def insert(self, key, value):
        index = self.hash1(key)
        step = self.hash2(key)
        start = index

        while self.table[index] is not None:
            self.collisions += 1
            index = (index + step) % self.size

            if index == start:
                raise Exception("Tabla llena")

        self.table[index] = (key, value)

    def search(self, key):
        index = self.hash1(key)
        step = self.hash2(key)
        start = index

        while self.table[index] is not None:
            if self.table[index][0] == key:
                return self.table[index][1]

            index = (index + step) % self.size

            if index == start:
                break

        return None

    def load_factor(self, total_elements):
        return total_elements / self.size


# =========================
# CARGA DE DATOS
# =========================
df = pd.read_csv("ecommerce_sample.csv")
df = df.dropna()

keys = df["Customer ID"].astype(str).head(10000).tolist()

table_size = 20011


# =========================
# PRUEBA ENCADENAMIENTO
# =========================
hash_chain = HashTableChaining(table_size)

start_time = time.time()
for key in keys:
    hash_chain.insert(key, {"Customer ID": key})
insert_time_chain = time.time() - start_time

start_time = time.time()
for key in keys[:1000]:
    hash_chain.search(key)
search_time_chain = time.time() - start_time


# =========================
# PRUEBA SONDEO LINEAL
# =========================
hash_linear = HashTableLinearProbing(table_size)

start_time = time.time()
for key in keys:
    hash_linear.insert(key, {"Customer ID": key})
insert_time_linear = time.time() - start_time

start_time = time.time()
for key in keys[:1000]:
    hash_linear.search(key)
search_time_linear = time.time() - start_time


# =========================
# PRUEBA DOBLE HASHING (NUEVO)
# =========================
hash_double = HashTableDoubleHashing(table_size)

start_time = time.time()
for key in keys:
    hash_double.insert(key, {"Customer ID": key})
insert_time_double = time.time() - start_time

start_time = time.time()
for key in keys[:1000]:
    hash_double.search(key)
search_time_double = time.time() - start_time


# =========================
# DICCIONARIO NATIVO
# =========================
native_dict = {}

start_time = time.time()
for key in keys:
    native_dict[key] = {"Customer ID": key}
insert_time_dict = time.time() - start_time

start_time = time.time()
for key in keys[:1000]:
    native_dict.get(key)
search_time_dict = time.time() - start_time


# =========================
# RESULTADOS
# =========================
results = pd.DataFrame({
    "Metodo": ["Encadenamiento", "Sondeo lineal", "Doble hashing", "Diccionario Python"],
    "Tiempo insercion": [insert_time_chain, insert_time_linear, insert_time_double, insert_time_dict],
    "Tiempo busqueda": [search_time_chain, search_time_linear, search_time_double, search_time_dict],
    "Colisiones": [hash_chain.collisions, hash_linear.collisions, hash_double.collisions, 0],
    "Factor de carga": [
        hash_chain.load_factor(len(keys)),
        hash_linear.load_factor(len(keys)),
        hash_double.load_factor(len(keys)),
        len(keys) / table_size
    ]
})

print(results)


# =========================
# GRAFICOS
# =========================
plt.figure(figsize=(10, 5))
plt.bar(results["Metodo"], results["Tiempo insercion"], color=['steelblue', 'coral', 'seagreen', 'gold'])
plt.title("Comparacion de tiempo de insercion")
plt.ylabel("Tiempo (segundos)")
plt.xticks(rotation=20)
plt.tight_layout()
plt.show()

plt.figure(figsize=(10, 5))
plt.bar(results["Metodo"], results["Tiempo busqueda"], color=['steelblue', 'coral', 'seagreen', 'gold'])
plt.title("Comparacion de tiempo de busqueda")
plt.ylabel("Tiempo (segundos)")
plt.xticks(rotation=20)
plt.tight_layout()
plt.show()

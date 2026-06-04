from collections import deque
from copy import deepcopy

# ==========================
# ESTRUCTURAS DE DATOS
# ==========================

hoja = []
indice_hash = {}
historial = []
cola_atencion = deque()


# ==========================
# FUNCIONES AUXILIARES
# ==========================

def calcular_promedio(n1, n2, n3):
    return (n1 + n2 + n3) / 3.0


def actualizar_hash():
    indice_hash.clear()

    for idx, estudiante in enumerate(hoja):
        indice_hash[estudiante["codigo"]] = idx


def guardar_historial():
    historial.append(deepcopy(hoja))


# ==========================
# CRUD ESTUDIANTES
# ==========================

def registrar_estudiante():
    print("\n--- REGISTRAR NUEVO ESTUDIANTE ---")

    codigo = input("Codigo unico: ").strip()

    if codigo in indice_hash:
        print("[ERROR] El codigo ya existe.")
        return

    nombre = input("Apellidos y Nombres: ").strip()

    try:
        nota1 = float(input("Nota 1 (0-20): "))
        nota2 = float(input("Nota 2 (0-20): "))
        nota3 = float(input("Nota 3 (0-20): "))
    except ValueError:
        print("[ERROR] Debe ingresar numeros validos.")
        return

    estudiante = {
        "codigo": codigo,
        "nombre": nombre,
        "nota1": nota1,
        "nota2": nota2,
        "nota3": nota3,
        "promedio": round(calcular_promedio(nota1, nota2, nota3), 2)
    }

    guardar_historial()

    hoja.append(estudiante)

    actualizar_hash()

    print("[EXITO] Estudiante registrado correctamente.")


def mostrar_hoja():

    if not hoja:
        print("\n[AVISO] No existen registros.")
        return

    print("\n" + "=" * 85)
    print(f"{'MINI HOJA DE CALCULO ACADEMICA':^85}")
    print("=" * 85)

    print(
        f"{'Codigo':<12}"
        f"{'Nombre':<30}"
        f"{'N1':<10}"
        f"{'N2':<10}"
        f"{'N3':<10}"
        f"{'Promedio':<10}"
    )

    print("-" * 85)

    for e in hoja:
        print(
            f"{e['codigo']:<12}"
            f"{e['nombre']:<30}"
            f"{e['nota1']:<10.1f}"
            f"{e['nota2']:<10.1f}"
            f"{e['nota3']:<10.1f}"
            f"{e['promedio']:<10.2f}"
        )

    print("=" * 85)


def buscar_estudiante():

    codigo = input(
        "\nIngrese codigo a buscar: "
    ).strip()

    if codigo in indice_hash:

        pos = indice_hash[codigo]

        e = hoja[pos]

        print("\n[ENCONTRADO]")
        print("Nombre   :", e["nombre"])
        print("Promedio :", e["promedio"])

    else:
        print("[AVISO] Codigo no encontrado.")


# ==========================
# QUICKSORT (PROMEDIO)
# ==========================

def quicksort(lista):

    if len(lista) <= 1:
        return lista

    pivote = lista[-1]

    mayores = [
        x for x in lista[:-1]
        if x["promedio"] >= pivote["promedio"]
    ]

    menores = [
        x for x in lista[:-1]
        if x["promedio"] < pivote["promedio"]
    ]

    return quicksort(mayores) + [pivote] + quicksort(menores)


# ==========================
# MERGESORT (CODIGO)
# ==========================

def merge_sort(lista):

    if len(lista) <= 1:
        return lista

    medio = len(lista) // 2

    izquierda = merge_sort(lista[:medio])
    derecha = merge_sort(lista[medio:])

    return merge(izquierda, derecha)


def merge(izquierda, derecha):

    resultado = []

    i = 0
    j = 0

    while i < len(izquierda) and j < len(derecha):

        if izquierda[i]["codigo"] <= derecha[j]["codigo"]:
            resultado.append(izquierda[i])
            i += 1
        else:
            resultado.append(derecha[j])
            j += 1

    resultado.extend(izquierda[i:])
    resultado.extend(derecha[j:])

    return resultado


# ==========================
# ORDENAMIENTO
# ==========================

def ordenar_por_promedio():

    global hoja

    if not hoja:
        print("[INFO] No hay datos.")
        return

    guardar_historial()

    hoja = quicksort(hoja)

    actualizar_hash()

    print(
        "[OK] Ordenado por promedio "
        "(QuickSort Descendente)."
    )


def ordenar_por_codigo():

    global hoja

    if not hoja:
        print("[INFO] No hay datos.")
        return

    guardar_historial()

    hoja = merge_sort(hoja)

    actualizar_hash()

    print(
        "[OK] Ordenado por codigo "
        "(MergeSort Ascendente)."
    )


# ==========================
# PILA (STACK)
# ==========================

def deshacer():

    global hoja

    if historial:

        hoja = historial.pop()

        actualizar_hash()

        print(
            "[REVERSION] "
            "Ultimo cambio restaurado."
        )

    else:
        print("[INFO] No hay historial.")


# ==========================
# COLA FIFO
# ==========================

def agregar_cola_atencion():

    codigo = input(
        "Codigo del estudiante: "
    ).strip()

    if codigo not in indice_hash:
        print("[ERROR] Codigo inexistente.")
        return

    cola_atencion.append(codigo)

    print(
        "[COLA] Estudiante agregado "
        "a la cola de espera."
    )


def atender_estudiante():

    if not cola_atencion:
        print("[INFO] Cola vacia.")
        return

    codigo = cola_atencion.popleft()

    pos = indice_hash[codigo]

    print(
        f"[ATENCION] "
        f"{hoja[pos]['nombre']}"
    )


# ==========================
# RECURSIVIDAD
# ==========================

def suma_promedios_recursiva(i):

    if i == len(hoja):
        return 0

    return (
        hoja[i]["promedio"]
        + suma_promedios_recursiva(i + 1)
    )


def estadisticas():

    if not hoja:
        print("[AVISO] No existen datos.")
        return

    suma = suma_promedios_recursiva(0)

    promedio_general = suma / len(hoja)

    mejor = max(
        hoja,
        key=lambda x: x["promedio"]
    )

    peor = min(
        hoja,
        key=lambda x: x["promedio"]
    )

    print("\n" + "=" * 40)

    print(
        f"Estudiantes registrados : "
        f"{len(hoja)}"
    )

    print(
        f"Promedio general        : "
        f"{promedio_general:.2f}"
    )

    print(
        f"Mejor estudiante        : "
        f"{mejor['nombre']} "
        f"({mejor['promedio']:.2f})"
    )

    print(
        f"Menor promedio          : "
        f"{peor['nombre']} "
        f"({peor['promedio']:.2f})"
    )

    print("=" * 40)


# ==========================
# MENU
# ==========================

def menu():

    while True:

        print("\n" + "=" * 45)
        print("MENU PRINCIPAL")
        print("=" * 45)

        print("1. Registrar estudiante")
        print("2. Mostrar hoja")
        print("3. Buscar estudiante")
        print("4. Ordenar por promedio")
        print("5. Ordenar por codigo")
        print("6. Deshacer")
        print("7. Agregar a cola")
        print("8. Atender estudiante")
        print("9. Estadisticas")
        print("10. Salir")

        print("-" * 45)

        opcion = input(
            "Seleccione opcion: "
        ).strip()

        if opcion == "1":
            registrar_estudiante()

        elif opcion == "2":
            mostrar_hoja()

        elif opcion == "3":
            buscar_estudiante()

        elif opcion == "4":
            ordenar_por_promedio()

        elif opcion == "5":
            ordenar_por_codigo()

        elif opcion == "6":
            deshacer()

        elif opcion == "7":
            agregar_cola_atencion()

        elif opcion == "8":
            atender_estudiante()

        elif opcion == "9":
            estadisticas()

        elif opcion == "10":
            print("\nFinalizando programa...")
            break

        else:
            print("[ERROR] Opcion invalida.")


# ==========================
# INICIO
# ==========================

if __name__ == "__main__":
    menu()

import random
import time

# COUNTING SORT (para Radix) 
def counting_sort(arr, exp):
    n = len(arr)
    output = [0] * n
    count = [0] * 10

    for i in range(n):
        index = (arr[i] // exp) % 10
        count[index] += 1

    for i in range(1, 10):
        count[i] += count[i - 1]

    i = n - 1
    while i >= 0:
        index = (arr[i] // exp) % 10
        output[count[index] - 1] = arr[i]
        count[index] -= 1
        i -= 1

    for i in range(n):
        arr[i] = output[i]

#  RADIX SORT 
def radix_sort(arr):
    max_num = max(arr)
    exp = 1
    while max_num // exp > 0:
        counting_sort(arr, exp)
        exp *= 10
#  FUNCIÓN PARA MEDIR
def medir_tiempo(func, arr):
    inicio = time.time()
    func(arr)
    fin = time.time()
    return fin - inicio

#  MAIN 
n = 100000

print("=====  RANGO GRANDE =====")
arr = [random.randint(0, 100000) for _ in range(n)]

arr1 = arr.copy()
t_radix = medir_tiempo(radix_sort, arr1)

arr2 = arr.copy()
t_timsort = medir_tiempo(sorted, arr2)


print("Radix Sort:", t_radix)
print("Timsort:", t_timsort)

import random
import time

def bubble_sort(arr):
    n = len(arr)
    for i in range(n):
        swapped = False
        for j in range(0, n - i - 1):
            if arr[j] > arr[j + 1]:
                arr[j], arr[j + 1] = arr[j + 1], arr[j]
                swapped = True
        if not swapped:
            break

def selection_sort(arr):
    for i in range(len(arr) - 1):
        min_idx = i
        for j in range(i + 1, len(arr)):
            if arr[j] < arr[min_idx]:
                min_idx = j
        if min_idx != i:  # evitamos swap innecesario
            arr[i], arr[min_idx] = arr[min_idx], arr[i]

def insertion_sort(arr):
    for i in range(1, len(arr)):
        key = arr[i]
        j = i - 1
        while j >= 0 and arr[j] > key:
            arr[j + 1] = arr[j]
            j -= 1
        arr[j + 1] = key

def shell_sort(arr):
    n = len(arr)
    gap = n // 2
    while gap > 0:
        for i in range(gap, n):
            temp = arr[i]
            j = i
            while j >= gap and arr[j - gap] > temp:
                arr[j] = arr[j - gap]
                j -= gap
            arr[j] = temp
        gap //= 2

def test(data, func, name):
    arr = data.copy()
    start = time.time()
    func(arr)
    end = time.time()
    print(f"{name} took {(end - start)*1e6:.2f} μs")

for n in [10, 100, 1000]:
    data = [random.randint(0, 10000) for _ in range(n)]
    print(f"--- Size: {n} ---")
    test(data, bubble_sort, "Bubble Sort")
    test(data, selection_sort, "Selection Sort")
    test(data, insertion_sort, "Insertion Sort")
    test(data, shell_sort, "Shell Sort")

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
using namespace std;

// COUNTING SORT 
void countingSort(vector<int>& arr, int maxVal) {
    vector<int> count(maxVal + 1, 0);
    vector<int> output(arr.size());

    for (int num : arr)
        count[num]++;
    for (int i = 1; i <= maxVal; i++)
        count[i] += count[i - 1];
    for (int i = arr.size() - 1; i >= 0; i--) {
        output[count[arr[i]] - 1] = arr[i];
        count[arr[i]]--;
    }
    arr = output;
}
int main() {
    int n = 1000;
    vector<int> arr(n);
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 100000; //rango que nos pide calcular la comparacion

    // COUNTING SORT 
    vector<int> arr1 = arr;
    clock_t t1 = clock();
    countingSort(arr1, 100000);//aqui tmabien se debe cambiar
    clock_t t2 = clock();
    cout << "Counting Sort: "
         << (double)(t2 - t1) / CLOCKS_PER_SEC << " s" << endl;
    // QUICKSORT (sort)
    vector<int> arr2 = arr;
    t1 = clock();
    sort(arr2.begin(), arr2.end());
    t2 = clock();
    cout << "QuickSort (sort): "
         << (double)(t2 - t1) / CLOCKS_PER_SEC << " s" << endl;

    //  MERGESORT (stable_sort) 
    vector<int> arr3 = arr;
    t1 = clock();
    stable_sort(arr3.begin(), arr3.end());
    t2 = clock();
    cout << "MergeSort (stable_sort): "
         << (double)(t2 - t1) / CLOCKS_PER_SEC << " s" << endl;

    return 0;
}

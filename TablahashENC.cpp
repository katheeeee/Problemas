#include <iostream>
#include <list>
#include <vector>
using namespace std;

class HashTable {
 int buckets;
 vector<list<int>> table;
 int hashFunction(int key) { return key % buckets; }

public:
 HashTable(int size) : buckets(size), table(size) {}

 void insertItem(int key) {
 int idx = hashFunction(key);
 table[idx].push_back(key);
 }

 bool searchItem(int key) {
 int idx = hashFunction(key);
 for (int val : table[idx])
 if (val == key) return true;
 return false;
 }

 void display() {
 for (int i = 0; i < buckets; i++) {
 cout << i << ": ";
 for (int val : table[i]) cout << val << " -> ";
 cout << "NULL\n";
 }
 }
};

int main() {
 // Se modifica el tamano de la tabla a m = 3 conforme a lo solicitado
 HashTable ht(3);

 ht.insertItem(15);
 ht.insertItem(11);
 ht.insertItem(27);
 ht.insertItem(8);
 ht.insertItem(12);

 ht.display();

 // Se removieron tildes y signos especiales para evitar caracteres raros en consola
 cout << "Existe 27? " << (ht.searchItem(27) ? "Si" : "No") << endl;

 return 0;
}

#include <iostream>
using namespace std;

// Orden mínimo del B-Tree (grado)
const int T = 3;

class BTreeNode {
public:
    int keys[2 * T - 1]; // Claves en el nodo
    BTreeNode* children[2 * T];
    int n; // Número de claves actuales
    bool leaf; // Verdadero si es hoja

    BTreeNode(bool _leaf) {
        leaf = _leaf;
        n = 0;
        for (int i = 0; i < 2 * T; i++) children[i] = nullptr;
    }

    // Recorrido in-order
    void traverse() {
        int i;
        for (i = 0; i < n; i++) {
            if (!leaf) children[i]->traverse();
            cout << keys[i] << " ";
        }
        if (!leaf) children[i]->traverse();
    }

    BTreeNode* search(int k) {
        int i = 0;
        while (i < n && k > keys[i]) i++;
        if (i < n && keys[i] == k) return this;
        if (leaf) return nullptr;
        return children[i]->search(k);
    }

    // Funciones auxiliares para inserción
    void insertNonFull(int k);
    void splitChild(int i, BTreeNode* y);

    // --- FUNCIONES PARA ELIMINACIÓN (TAREA OPCIONAL) ---
    int findKey(int k);
    void remove(int k);
    void removeFromLeaf(int idx);
    void removeFromNonLeaf(int idx);
    int getPred(int idx);
    int getSucc(int idx);
    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);
};

class BTree {
public:
    BTreeNode* root;
    BTree() { root = nullptr; }

    void traverse() {
        if (root) root->traverse();
        cout << endl;
    }

    BTreeNode* search(int k) {
        return (root == nullptr) ? nullptr : root->search(k);
    }

    void insert(int k);
    void remove(int k); // Envoltura para eliminar
};

// --- IMPLEMENTACIÓN DE INSERCIÓN ---
void BTreeNode::insertNonFull(int k) {
    int i = n - 1;
    if (leaf) {
        while (i >= 0 && keys[i] > k) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = k;
        n++;
    } else {
        while (i >= 0 && keys[i] > k) i--;
        if (children[i + 1]->n == 2 * T - 1) {
            splitChild(i + 1, children[i + 1]);
            if (keys[i + 1] < k) i++;
        }
        children[i + 1]->insertNonFull(k);
    }
}

void BTreeNode::splitChild(int i, BTreeNode* y) {
    BTreeNode* z = new BTreeNode(y->leaf);
    z->n = T - 1;
    for (int j = 0; j < T - 1; j++) z->keys[j] = y->keys[j + T];
    if (!y->leaf) {
        for (int j = 0; j < T; j++) z->children[j] = y->children[j + T];
    }
    y->n = T - 1;
    for (int j = n; j >= i + 1; j--) children[j + 1] = children[j];
    children[i + 1] = z;
    for (int j = n - 1; j >= i; j--) keys[j + 1] = keys[j];
    keys[i] = y->keys[T - 1];
    n++;
}

void BTree::insert(int k) {
    if (root == nullptr) {
        root = new BTreeNode(true);
        root->keys[0] = k;
        root->n = 1;
    } else {
        if (root->n == 2 * T - 1) {
            BTreeNode* s = new BTreeNode(false);
            s->children[0] = root;
            s->splitChild(0, root);
            int i = 0;
            if (s->keys[0] < k) i++;
            s->children[i]->insertNonFull(k);
            root = s;
        } else {
            root->insertNonFull(k);
        }
    }
}

// --- IMPLEMENTACIÓN DE ELIMINACIÓN ---
int BTreeNode::findKey(int k) {
    int idx = 0;
    while (idx < n && keys[idx] < k) ++idx;
    return idx;
}

void BTreeNode::remove(int k) {
    int idx = findKey(k);
    if (idx < n && keys[idx] == k) {
        if (leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
    } else {
        if (leaf) {
            cout << "La clave " << k << " no existe en el arbol.\n";
            return;
        }
        bool flag = ((idx == n));
        if (children[idx]->n < T) fill(idx);
        if (flag && idx > n)
            children[idx - 1]->remove(k);
        else
            children[idx]->remove(k);
    }
}

void BTreeNode::removeFromLeaf(int idx) {
    for (int i = idx + 1; i < n; ++i) keys[i - 1] = keys[i];
    n--;
}

void BTreeNode::removeFromNonLeaf(int idx) {
    int k = keys[idx];
    if (children[idx]->n >= T) {
        int pred = getPred(idx);
        keys[idx] = pred;
        children[idx]->remove(pred);
    } else if (children[idx + 1]->n >= T) {
        int succ = getSucc(idx);
        keys[idx] = succ;
        children[idx + 1]->remove(succ);
    } else {
        merge(idx);
        children[idx]->remove(k);
    }
}

int BTreeNode::getPred(int idx) {
    BTreeNode* cur = children[idx];
    while (!cur->leaf) cur = cur->children[cur->n];
    return cur->keys[cur->n - 1];
}

int BTreeNode::getSucc(int idx) {
    BTreeNode* cur = children[idx + 1];
    while (!cur->leaf) cur = cur->children[0];
    return cur->keys[0];
}

void BTreeNode::fill(int idx) {
    if (idx != 0 && children[idx - 1]->n >= T)
        borrowFromPrev(idx);
    else if (idx != n && children[idx + 1]->n >= T)
        borrowFromNext(idx);
    else {
        if (idx != n)
            merge(idx);
        else
            merge(idx - 1);
    }
}

void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx - 1];
    for (int i = child->n - 1; i >= 0; --i) child->keys[i + 1] = child->keys[i];
    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) child->children[i + 1] = child->children[i];
    }
    child->keys[0] = keys[idx - 1];
    if (!child->leaf) child->children[0] = sibling->children[sibling->n];
    keys[idx - 1] = sibling->keys[sibling->n - 1];
    child->n += 1;
    sibling->n -= 1;
}

void BTreeNode::borrowFromNext(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];
    child->keys[(child->n)] = keys[idx];
    if (!child->leaf) child->children[(child->n) + 1] = sibling->children[0];
    keys[idx] = sibling->keys[0];
    for (int i = 1; i < sibling->n; ++i) sibling->keys[i - 1] = sibling->keys[i];
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i) sibling->children[i - 1] = sibling->children[i];
    }
    child->n += 1;
    sibling->n -= 1;
}

void BTreeNode::merge(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];
    child->keys[T - 1] = keys[idx];
    for (int i = 0; i < sibling->n; ++i) child->keys[i + T] = sibling->keys[i];
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i) child->children[i + T] = sibling->children[i];
    }
    for (int i = idx + 1; i < n; ++i) keys[i - 1] = keys[i];
    for (int i = idx + 2; i <= n; ++i) children[i - 1] = children[i];
    child->n += sibling->n + 1;
    n--;
    delete (sibling);
}

void BTree::remove(int k) {
    if (!root) {
        cout << "El arbol esta vacio.\n";
        return;
    }
    root->remove(k);
    if (root->n == 0) {
        BTreeNode* tmp = root;
        if (root->leaf)
            root = nullptr;
        else
            root = root->children[0];
        delete tmp;
    }
}

// --- MAIN (Puntos 1, 2, 3 y 4) ---
int main() {
    BTree t;

    // 2. Insertar un conjunto de 15-20 números (18 elementos aquí)
    int valores[] = {10, 20, 5, 6, 12, 30, 7, 17, 3, 4, 15, 18, 22, 25, 40, 50, 11, 14};
    cout << "== 1. Insertando 18 elementos en el B-Tree ==\n";
    for (int v : valores) {
        t.insert(v);
    }

    // 4. Verificar el recorrido in-order (Debe salir perfectamente ordenado)
    cout << "Recorrido In-Order actual: ";
    t.traverse();

    // Búsqueda de prueba
    int k = 12;
    cout << "\nBuscando el elemento [" << k << "]: ";
    (t.search(k) != nullptr) ? cout << "Encontrado" : cout << "No encontrado";
    cout << "\n\n";

    // 3. Probar la operación de eliminación (Caso Opcional Implementado)
    cout << "== 2. Probando eliminaciones (Balanceo automatico) ==\n";

    cout << "Eliminando 6 (Elemento en hoja)...\n";
    t.remove(6);
    cout << "Recorrido In-Order: ";
    t.traverse();

    cout << "\nEliminando 10 (Clave interna, requiere predecesor/sucesor o fusion)...\n";
    t.remove(10);
    cout << "Recorrido In-Order: ";
    t.traverse();

    cout << "\nEliminando 20 (Otra clave interna)...\n";
    t.remove(20);
    cout << "Recorrido In-Order: ";
    t.traverse();

    return 0;
}

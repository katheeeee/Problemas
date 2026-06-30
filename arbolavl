#include <iostream>
#include <algorithm> // Necesario para la funcion max
using namespace std;

struct Node {
 int key, height;
 Node *left, *right;
 Node(int val): key(val), height(1), left(nullptr), right(nullptr) {}
};

int getHeight(Node* n) { return n ? n->height : 0; }
int getBalance(Node* n) { return n ? getHeight(n->left) - getHeight(n->right) : 0; }

Node* rightRotate(Node* y) {
 Node* x = y->left;
 Node* T2 = x->right;
 x->right = y;
 y->left = T2;
 y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
 x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
 return x;
}

Node* leftRotate(Node* x) {
 Node* y = x->right;
 Node* T2 = y->left;
 y->left = x;
 x->right = T2;
 x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
 y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
 return y;
}

Node* insert(Node* node, int key) {
 if (!node) return new Node(key);
 if (key < node->key) node->left = insert(node->left, key);
 else if (key > node->key) node->right = insert(node->right, key);
 else return node;

 node->height = 1 + max(getHeight(node->left), getHeight(node->right));
 int balance = getBalance(node);

 if (balance > 1 && key < node->left->key) return rightRotate(node);
 if (balance < -1 && key > node->right->key) return leftRotate(node);
 if (balance > 1 && key > node->left->key) { node->left = leftRotate(node->left); return rightRotate(node); }
 if (balance < -1 && key < node->right->key) { node->right = rightRotate(node->right); return leftRotate(node); }
 return node;
}

void preOrder(Node* root) {
 if (root) {
 cout << root->key << " ";
 preOrder(root->left);
 preOrder(root->right);
 }
}

int main() {
 Node* root = nullptr;

 // Se expande la insercion a un caso de prueba real con 15 elementos
 root = insert(root, 10);
 root = insert(root, 20);
 root = insert(root, 30);
 root = insert(root, 40);
 root = insert(root, 50);
 root = insert(root, 60);
 root = insert(root, 70);
 root = insert(root, 80);
 root = insert(root, 90);
 root = insert(root, 100);
 root = insert(root, 110);
 root = insert(root, 120);
 root = insert(root, 130);
 root = insert(root, 140);
 root = insert(root, 150);

 // Se eliminaron tildes para asegurar la correcta lectura en la terminal
 cout << "Preorden del arbol AVL con 15 elementos: " << endl;
 preOrder(root);
 cout << endl;

 return 0;
}

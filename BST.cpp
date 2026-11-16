#include "BST.h"

BST::BST() : root(nullptr) {}

BST::~BST() {
    destroy(root);
}

void BST::destroy(TreeNode* node) {
    if (node == nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

void BST::updateHeight(TreeNode* node) {
    int left = heightOf(node->left);
    int right = heightOf(node->right);
    node->height = 1 + (left > right ? left : right);
}

TreeNode* BST::insertNode(TreeNode* node, int key, Parcel* parcel) {
    if (node == nullptr) return new TreeNode(key, parcel);

    if (key < node->key) node->left = insertNode(node->left, key, parcel);
    else if (key > node->key) node->right = insertNode(node->right, key, parcel);
    else return node;   // duplicate key

    updateHeight(node);
    return balance(node);
}

void BST::insert(int key, Parcel* parcel) {
    root = insertNode(root, key, parcel);
}

Parcel* BST::search(int key) const {
    TreeNode* current = root;
    while (current != nullptr) {
        if (key == current->key) return current->parcel;
        current = (key < current->key) ? current->left : current->right;
    }
    return nullptr;
}

void BST::inorderVisit(TreeNode* node, void (*visit)(const Parcel&)) const {
    if (node == nullptr) return;
    inorderVisit(node->left, visit);
    visit(*node->parcel);
    inorderVisit(node->right, visit);
}

void BST::inorder(void (*visit)(const Parcel&)) const {
    inorderVisit(root, visit);
}

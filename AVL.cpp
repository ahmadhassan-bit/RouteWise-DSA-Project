#include "AVL.h"

TreeNode* AVL::rotateRight(TreeNode* node) {
    TreeNode* newRoot = node->left;
    node->left = newRoot->right;
    newRoot->right = node;
    updateHeight(node);
    updateHeight(newRoot);
    return newRoot;
}

TreeNode* AVL::rotateLeft(TreeNode* node) {
    TreeNode* newRoot = node->right;
    node->right = newRoot->left;
    newRoot->left = node;
    updateHeight(node);
    updateHeight(newRoot);
    return newRoot;
}

TreeNode* AVL::balance(TreeNode* node) {
    int balanceFactor = heightOf(node->left) - heightOf(node->right);

    if (balanceFactor > 1) {                                   // left side too tall
        if (heightOf(node->left->left) < heightOf(node->left->right)) {
            node->left = rotateLeft(node->left);               // left-right case
        }
        return rotateRight(node);
    }
    if (balanceFactor < -1) {                                  // right side too tall
        if (heightOf(node->right->right) < heightOf(node->right->left)) {
            node->right = rotateRight(node->right);            // right-left case
        }
        return rotateLeft(node);
    }
    return node;
}

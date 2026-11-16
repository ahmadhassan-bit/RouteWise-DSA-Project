#ifndef BST_H
#define BST_H

#include "Models.h"

struct TreeNode {
    int key;           // tracking ID
    Parcel* parcel;    // not owned by the tree
    TreeNode* left;
    TreeNode* right;
    int height;

    TreeNode(int key, Parcel* parcel)
        : key(key), parcel(parcel), left(nullptr), right(nullptr), height(1) {}
};

// Plain binary search tree. The AVL tree reuses it and only adds balancing.
class BST {
protected:
    TreeNode* root;

    static int heightOf(TreeNode* node) { return node == nullptr ? 0 : node->height; }
    static void updateHeight(TreeNode* node);

    // Called after every insertion step; the BST does nothing, the AVL tree rebalances here.
    virtual TreeNode* balance(TreeNode* node) { return node; }

private:
    TreeNode* insertNode(TreeNode* node, int key, Parcel* parcel);
    void inorderVisit(TreeNode* node, void (*visit)(const Parcel&)) const;
    void destroy(TreeNode* node);

public:
    BST();
    virtual ~BST();

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    void insert(int key, Parcel* parcel);   // duplicate keys are ignored
    Parcel* search(int key) const;
    void inorder(void (*visit)(const Parcel&)) const;   // visits parcels in ascending key order
    int height() const { return heightOf(root); }
};

#endif

#ifndef AVL_H
#define AVL_H

#include "BST.h"

// Self-balancing version of the BST: after each insertion the tree is rotated
// so that the heights of the two subtrees of any node differ by at most 1.
class AVL : public BST {
private:
    TreeNode* rotateLeft(TreeNode* node);
    TreeNode* rotateRight(TreeNode* node);

protected:
    TreeNode* balance(TreeNode* node) override;
};

#endif

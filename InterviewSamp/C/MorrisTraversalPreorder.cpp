// C program for Morris Preorder traversal
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Header.h"

// Preorder traversal without recursion and without stack
void morrisTraversalPreorder(struct node* root)
{
    while (root)
    {
        // If left child is null, print the current node data. Move to
        // right child.
        if (root->left == NULL)
        {
            printf("%d ", root->data);
            root = root->right;
        }
        else
        {
            // Find inorder predecessor
            struct node* current = root->left;
            while (current->right && current->right != root)
                current = current->right;

            // If the right child of inorder predecessor already points to
            // this node
            if (current->right == root)
            {
                current->right = NULL;
                root = root->right;
            }

            // If right child doesn't point to this node, then print this
            // node and make right child point to this node
            else
            {
                printf("%d ", root->data);
                current->right = root;
                root = root->left;
            }
        }
    }
}

// Function for sStandard preorder traversal
void preorder(struct node* root)
{
    if (root)
    {
        printf("%d ", root->data);
        preorder(root->left);
        preorder(root->right);
    }
}
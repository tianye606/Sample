#include "stdafx.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include "Header.h"


/* Helper function that allocates a new node with the
given data and NULL left and right pointers. */
struct node* newNode(int data)
{
    struct node* temp = (struct node*) malloc(sizeof(struct node));
    temp->data = data;
    temp->left = temp->right = NULL;
    return temp;
}

int _tmain(int argc, _TCHAR* argv[])
{
    //int A[5] = { 1, 2, 3, 4, 5 };
    //int B[4] = { 6, 7, 8, 9 };
    //int res = findKthSmallest(A, _countof(A), B, _countof(B), 4);
    //printf("%d", res);

    //char res[256] = { 0 };
    //BreakStrInLeastDistance("aabbcc", 2, res);
    //printf("%s", res);
    
    //int res = climbStairs(3);
    //printf("%d", res);
    
    //struct node* root = NULL;

    //root = newNode(1);
    //root->left = newNode(2);
    //root->right = newNode(3);

    //root->left->left = newNode(4);
    //root->left->right = newNode(5);

    //root->right->left = newNode(6);
    //root->right->right = newNode(7);

    //root->left->left->left = newNode(8);
    //root->left->left->right = newNode(9);

    //root->left->right->left = newNode(10);
    //root->left->right->right = newNode(11);

    //morrisTraversalPreorder(root);

    //printf("\n");
    //preorder(root);

    struct node* root = NULL;

    root = newNode(1);
    root->left = newNode(2);
    root->right = newNode(3);

    root->left->left = newNode(4);
    root->left->right = newNode(5);

    root->right->left = newNode(6);
    root->right->right = newNode(7);

    root->left->left->left = newNode(8);
    root->left->left->right = newNode(9);

    root->left->right->left = newNode(10);
    root->left->right->right = newNode(11);

    int depth=minDepth(root,false);
    return 0;

}
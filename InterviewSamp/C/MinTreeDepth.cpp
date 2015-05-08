#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Header.h"

//Depth is the distance from root to leave node, so if a node is null but it has brother, its distance should be ignored by set INT_MAX
int minDepth(const node *root, bool hasbrother)
{
    if (!root) return hasbrother ? INT_MAX : 0;
    int left = minDepth(root->left, root->right != NULL);
    int right = minDepth(root->right, root->left != NULL);
    int m = min(left, right);
    return 1 + m;
}
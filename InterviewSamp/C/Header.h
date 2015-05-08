int findKthSmallest(int A[], int m, int B[], int n, int k);
void BreakStrInLeastDistance(char* str, int d, char ans[]);
int climbStairs(int n);
int canCompleteCircuit(int gas[], int cost[], int len);
int WaterTrap(int A[], int n);

//Tree
void morrisTraversalPreorder(struct node* root);
void preorder(struct node* root);
int minDepth(const node *root, bool hasbrother);

struct node
{
    int data;
    struct node *left, *right;
};
#define max(a, b) a > b ? a: b
#define min(a, b) a < b ? a: b

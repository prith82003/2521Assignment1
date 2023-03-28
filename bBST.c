
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bBST.h"
#include "List.h"

////////////////////////////////////////////////////////////////////////
/* IMPORTANT:
   Do NOT modify the code above this line.
*/
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

// Auxiliary function prototypes
static void FreeNode(Node n);
static bool NodeSearch(Node n, int k);
static Node NodeCreate(int k);
static Node NodeInsert(Node curr, Node n);
static int GetBalance(Node n);
static Node LeftLeftCase(Node x, Node y, Node z);
static Node LeftRightCase(Node x, Node y, Node z);
static Node RightRightCase(Node x, Node y, Node z);
static Node RightLeftCase(Node x, Node y, Node z);
static int Height(Node n);
static void UpdateHeight(Node n);
static int max(int a, int b);
static Node NodeDelete(Node n, int key);
static Node BalanceTree(Node curr, Node n, bool (*CheckCase)(Node, int));
static Node GetNextNode(Node n);
static bool CheckInsertCase(Node curr, int nKey);
static bool CheckDeleteCase(Node curr, int leftCase);
static void NodeToList(List l, Node curr);
static Node NodeKthSmallest(Node curr, int k, int *count);
static Node NodeKthLargest(Node curr, int k, int *count);
static Node NodeLCA(Node curr, int a, int b);
static Node NodeFloor(Node curr, int key);
static Node NodeCeiling(Node curr, int key);
static void NodeSearchBetween(Node curr, int lower, int upper, List l);

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

/**
 * Creates a new empty tree.
 * The time complexity of this function must be O(1).
 */
Tree TreeNew(void)
{
	Tree t = malloc(sizeof(*t));

	if (t == NULL)
	{
		fprintf(stderr, "Could not malloc Tree\n");
		exit(EXIT_FAILURE);
	}

	t->root = NULL;
	return t;
}

////////////////////////////////////////////////////////////////////////

/**
 * Frees all memory allocated for the given tree.
 * The time complexity of this function must be O(n).
 */
void TreeFree(Tree t)
{
	if (t == NULL)
		return;

	if (t->root != NULL)
		FreeNode(t->root);

	free(t);
}

/**
 * Recursively Delete all nodes in the tree
 */
static void FreeNode(Node n)
{
	if (n == NULL)
		return;

	// Hold left and right in temp variables
	// Then free current node and recursively call
	// function on both children

	Node left = n->left;
	Node right = n->right;

	free(n);

	FreeNode(left);
	FreeNode(right);
}

////////////////////////////////////////////////////////////////////////

/**
 * Searches the tree for a given key and returns true if the key is in
 * the tree or false otherwise.
 * The time complexity of this function must be O(log n).
 */
bool TreeSearch(Tree t, int key)
{
	// Early return false if t is empty or NULL
	if (t == NULL)
		return false;
	if (t->root == NULL)
		return false;

	if (key == UNDEFINED)
		return false;

	return NodeSearch(t->root, key);
}

/**
 * Use standard BST Search to search for given key in BST
 */
static bool NodeSearch(Node n, int k)
{
	if (n == NULL)
		return false;

	if (n->key == k)
		return true;

	return (k > n->key) ? NodeSearch(n->right, k) : NodeSearch(n->left, k);
}

////////////////////////////////////////////////////////////////////////

/**
 * Inserts the given key into the tree.
 * You must use the proper AVL insertion algorithm.
 * Returns true if the key was inserted successfully, or false if the
 * key was already present in the tree.
 * The time complexity of this function must be O(log n).
 */
bool TreeInsert(Tree t, int key)
{
	if (t == NULL)
		return false;

	// Check if key already exists in tree
	if (TreeSearch(t, key))
	{
		fprintf(stderr, "Value %d already Exists in Tree\n", key);
		return false;
	}

	// Check if key is undefined
	if (key == UNDEFINED)
	{
		fprintf(stderr, "Can't Insert Undefined Value\n");
		return false;
	}

	Node new = NodeCreate(key);

	// Assign to root if tree is empty
	if (t->root == NULL)
	{
		t->root = new;
		return true;
	}

	// Insert new node into tree
	t->root = NodeInsert(t->root, new);
	return true;
}

/**
 * Create New node and set all properties
 */
static Node NodeCreate(int k)
{
	Node n = malloc(sizeof(*n));

	if (n == NULL)
	{
		fprintf(stderr, "Could not malloc Node\n");
		exit(EXIT_FAILURE);
	}

	n->key = k;
	n->left = NULL;
	n->right = NULL;
	n->height = 0;

	return n;
}

/**
 * Search for correct position to insert new node
 * Balance tree if necessary
 */
static Node NodeInsert(Node curr, Node n)
{
	// If the end of the tree is reached, insert the new node
	if (curr == NULL)
		return n;

	// Search for the correct position in the BST
	if (curr->key > n->key)
		curr->left = NodeInsert(curr->left, n);
	else
		curr->right = NodeInsert(curr->right, n);

	// Balance the tree if necessary
	return BalanceTree(curr, n, CheckInsertCase);
}

/**
 * Balance the tree if necessary
 * Update height of current node
 */
static Node BalanceTree(Node curr, Node n, bool (*CheckCase)(Node, int))
{
	// Get balance factor to know which case to check
	int balanceFactor = GetBalance(curr);
	bool balanced = (abs(balanceFactor) <= 1);

	if (!balanced)
	{
		Node x = NULL;
		Node y = NULL;
		Node z = curr;

		// Left cases if balance factor is greater than 1
		if (balanceFactor > 1)
		{
			// Check which left case using function pointer
			int nKey = (n == NULL) ? 1 : n->key;
			if (CheckCase(curr->left, nKey))
			{
				y = z->left;
				x = y->left;
				return LeftLeftCase(x, y, z);
			}

			y = z->left;
			x = y->right;
			return LeftRightCase(x, y, z);
		} // Right cases if balance factor is less than -1
		else
		{
			int nKey = (n == NULL) ? 0 : n->key;
			if (!CheckCase(curr->right, nKey))
			{
				y = z->right;
				x = y->right;
				return RightRightCase(x, y, z);
			}

			y = z->right;
			x = y->left;
			return RightLeftCase(x, y, z);
		}
	}

	// Update current node's height
	UpdateHeight(curr);
	return curr;
}

/**
 * Check which case to use for insertion
 */
static int GetBalance(Node n)
{
	return Height(n->left) - Height(n->right);
}

/**
 * Left Left Case of rotation
 */
static Node LeftLeftCase(Node x, Node y, Node z)
{
	// Perform a left rotation on the BST based around node y
	z->left = y->right;
	y->right = z;

	UpdateHeight(z);
	UpdateHeight(y);

	return y;
}

/**
 * Left Right Case of rotation
 */
static Node LeftRightCase(Node x, Node y, Node z)
{
	// Perform a left rotation, then right rotation based on node x
	z->left = x->right;
	x->right = z;
	y->right = x->left;
	x->left = y;

	UpdateHeight(z);
	UpdateHeight(y);
	UpdateHeight(x);

	return x;
}

/**
 * Right Right Case of rotation
 */
static Node RightRightCase(Node x, Node y, Node z)
{
	// Perform a right rotation based on node y
	z->right = y->left;
	y->left = z;

	UpdateHeight(z);
	UpdateHeight(y);
	return y;
}

/**
 * Right Left Case of rotation
 */
static Node RightLeftCase(Node x, Node y, Node z)
{
	// Perform a right rotation, then left rotation based on node x
	y->left = x->right;
	x->right = y;
	z->right = x->left;
	x->left = z;

	UpdateHeight(z);
	UpdateHeight(y);
	UpdateHeight(x);

	return x;
}

/**
 * Check which case to use for insertion
 */
static bool CheckInsertCase(Node curr, int nKey)
{
	// Condition for the Left Left and Right Right cases
	return nKey < curr->key;
}

/**
 * Update height of node
 */
static void UpdateHeight(Node n)
{
	n->height = 1 + max(Height(n->left), Height(n->right));
}

////////////////////////////////////////////////////////////////////////

/**
 * Deletes the given key from the tree if it is present.
 * You must use the proper AVL deletion algorithm.
 * Returns true if the key was deleted successfully, or false if the key
 * was not present in the tree.
 * The time complexity of this function must be O(log n).
 */
bool TreeDelete(Tree t, int key)
{
	// Check for invalid cases
	if (t == NULL)
		return false;

	if (t->root == NULL)
		return false;

	if (key == UNDEFINED)
	{
		fprintf(stderr, "Can't accept UNDEFINED as input\n");
		return false;
	}

	// Can't delete if not in tree
	if (!TreeSearch(t, key))
	{
		fprintf(stderr, "Value to Delete not in Tree\n");
		return false;
	}

	// Delete the node
	t->root = NodeDelete(t->root, key);
	return true;
}

/**
 * Search for the node to delete
 * Balance the tree if necessary
 */
static Node NodeDelete(Node curr, int key)
{
	if (curr == NULL)
		return NULL;

	// Search for node to be deleted
	if (key > curr->key)
		curr->right = NodeDelete(curr->right, key);
	else if (key < curr->key)
		curr->left = NodeDelete(curr->left, key);
	else
		// Delete node and find node to replace it with
		curr = GetNextNode(curr);

	// If a leaf node has been deleted, return NULL
	if (curr == NULL)
		return curr;

	// Balance Tree if necessary
	return BalanceTree(curr, NULL, CheckDeleteCase);
}

/**
 * Get the next node to replace the deleted node
 */
static Node GetNextNode(Node n)
{
	// One/Zero Child Cases
	if (n->right == NULL)
	{
		// If no right child, replace with left child even if NULL
		Node tempLeft = n->left;
		free(n);
		return tempLeft;
	}
	else if (n->left == NULL)
	{
		// If Only right child, replace with right child
		Node tempRight = n->right;
		free(n);
		return tempRight;
	}

	// Both Child Case
	// Get smallest Node in right subtree, replace current node
	// Delete that node in right subtree
	int count = 0;
	Node min = NodeKthSmallest(n->right, 1, &count);
	n->key = min->key;

	n->right = NodeDelete(n->right, min->key);
	return n;
}

/**
 * Check which case to use for deletion
 */
static bool CheckDeleteCase(Node sub, int leftCase)
{
	// Condition for the Left Left and Right Right cases
	// Right Right Case is inverted since opposite condition is used in Right Right condition
	int balanceSub = GetBalance(sub);
	return (leftCase) ? balanceSub >= 0 : balanceSub > 0;
}

////////////////////////////////////////////////////////////////////////

/**
 * Creates a list containing all the keys in the given tree in ascending
 * order.
 * The time complexity of this function must be O(n).
 */
List TreeToList(Tree t)
{
	List l = ListNew();
	if (t == NULL)
		return l;
	NodeToList(l, t->root);
	return l;
}

/**
 * In-order traverse the tree and add the keys to the list
 */
static void NodeToList(List l, Node curr)
{
	if (curr == NULL)
		return;

	NodeToList(l, curr->left);
	ListAppend(l, curr->key);
	NodeToList(l, curr->right);
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

/**
 * Returns the k-th smallest key in the tree.
 * Assumes that k is between 1 and the number of nodes in the tree.
 * k = 1 will return the smallest value in the tree.
 * The time complexity of this function must be O(log n + k).
 */
int TreeKthSmallest(Tree t, int k)
{
	if (t == NULL)
		return UNDEFINED;

	if (t->root == NULL)
		return UNDEFINED;

	// Initialise variable so pointer can be passed to recursive function
	// Pointer is used so that variable is same on all recursive calls
	int count = 0;
	Node result = NodeKthSmallest(t->root, k, &count);

	// Return undefined if node is not found
	return (result == NULL) ? UNDEFINED : result->key;
}

/**
 * In order traverse through BST to find the kth smallest node
 */
static Node NodeKthSmallest(Node curr, int k, int *count)
{
	if (curr == NULL)
		return NULL;

	// In-order traverse through BST
	Node left = NodeKthSmallest(curr->left, k, count);

	if (left != NULL)
		return left;

	// Count number of non-null nodes visited
	(*count)++;

	// Once kth smallest node is found, return it
	if (*count == k)
		return curr;

	return NodeKthSmallest(curr->right, k, count);
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the k-th largest key in the tree.
 * Assumes that k is between 1 and the number of nodes in the tree.
 * k = 1 will return the largest value in the tree.
 * The time complexity of this function must be O(log n + k).
 */
int TreeKthLargest(Tree t, int k)
{
	if (t == NULL)
		return UNDEFINED;

	if (t->root == NULL)
		return UNDEFINED;

	int count = 0;
	Node result = NodeKthLargest(t->root, k, &count);

	return (result == NULL) ? UNDEFINED : result->key;
}

/**
 * Recursively traverse through the BST to find the kth largest node
 */
static Node NodeKthLargest(Node curr, int k, int *count)
{
	if (curr == NULL)
		return NULL;

	// Reach the largest node in the BST
	Node right = NodeKthLargest(curr->right, k, count);

	if (right != NULL)
		return right;

	// Count backwards from the tree size to find the kth largest node
	(*count)++;
	if (*count == k)
		return curr;

	return NodeKthLargest(curr->left, k, count);
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the least common ancestor of two keys, a and b.
 * Returns UNDEFINED if either a or b are not present in the tree.
 * a may be larger than b, or equal to each other.
 * Either a or b may be the LCA in some cases.
 * The time complexity of this function must be O(log n).
 */
int TreeLCA(Tree t, int a, int b)
{
	if (t == NULL)
		return UNDEFINED;

	if (t->root == NULL)
		return UNDEFINED;

	if (!TreeSearch(t, a) || !TreeSearch(t, b))
		return UNDEFINED;

	Node result = NodeLCA(t->root, a, b);
	return (result == NULL) ? UNDEFINED : result->key;
}

/**
 * Recursively traverse through the BST to find the LCA of two nodes
 */
static Node NodeLCA(Node curr, int a, int b)
{
	if (curr == NULL)
		return NULL;

	// Check if a and b are on different sides of the tree
	bool ALeft = a < curr->key;
	bool BLeft = b < curr->key;

	// Check if a or b is the current node
	bool isA = curr->key == a;
	bool isB = curr->key == b;

	// If a and b are on different sides of the tree, then the current node is the LCA
	// If a or b is the current node, then the current node is the LCA
	if (ALeft != BLeft || isA || isB)
		return curr;
	// If a and b are on the same side of the tree, then traverse down that side
	return (ALeft) ? NodeLCA(curr->left, a, b) : NodeLCA(curr->right, a, b);
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the largest key less than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */
int TreeFloor(Tree t, int key)
{
	if (t == NULL)
		return UNDEFINED;

	if (t->root == NULL)
		return UNDEFINED;

	Node result = NodeFloor(t->root, key);
	return (result == NULL) ? UNDEFINED : result->key;
}

static Node NodeFloor(Node curr, int key)
{
	if (curr == NULL)
		return NULL;

	// Floor should return key if it is found
	if (curr->key == key)
		return curr;

	// If key is less than current node, then floor must be in the left subtree
	if (curr->key > key)
		return NodeFloor(curr->left, key);

	// If key is greater than current node, then floor must be in the right subtree
	Node right = NodeFloor(curr->right, key);
	return (right == NULL) ? curr : right;
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the smallest key greater than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */
int TreeCeiling(Tree t, int key)
{
	if (t == NULL)
		return UNDEFINED;

	if (t->root == NULL)
		return UNDEFINED;

	Node result = NodeCeiling(t->root, key);
	return (result == NULL) ? UNDEFINED : result->key;
}

static Node NodeCeiling(Node curr, int key)
{
	if (curr == NULL)
		return NULL;

	// Return key if it is found in BST
	if (curr->key == key)
		return curr;

	// If key is greater than current node, then ceiling must be in the right subtree
	if (curr->key < key)
		return NodeCeiling(curr->right, key);

	// If key is less than current node, then ceiling must be in the left subtree
	Node left = NodeCeiling(curr->left, key);
	return (left == NULL) ? curr : left;
}

////////////////////////////////////////////////////////////////////////

/**
 * Searches for all keys between the two given keys (inclusive) and
 * returns the keys in a list in ascending order.
 * Assumes that lower is less than or equal to upper.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 */
List TreeSearchBetween(Tree t, int lower, int upper)
{
	List l = ListNew();

	if (t == NULL)
		return l;

	if (lower > upper)
		return l;

	if (lower == UNDEFINED || upper == UNDEFINED)
		return l;

	NodeSearchBetween(t->root, lower, upper, l);
	return l;
}

static void NodeSearchBetween(Node curr, int lower, int upper, List l)
{
	if (curr == NULL)
		return;

	// Conditions to check if between lower and upper range
	bool isAbove = curr->key >= lower;
	bool isBelow = curr->key <= upper;
	bool isBetween = isAbove && isBelow;

	// If between, in-order traverse to find all keys between
	if (isBetween)
	{
		NodeSearchBetween(curr->left, lower, upper, l);
		ListAppend(l, curr->key);
		NodeSearchBetween(curr->right, lower, upper, l);
		return;
	}

	// If not between, then traverse down the correct side of the tree
	if (lower <= curr->key)
		NodeSearchBetween(curr->left, lower, upper, l);
	if (upper >= curr->key)
		NodeSearchBetween(curr->right, lower, upper, l);
}

////////////////////////////////////////////////////////////////////////

/* Helper Functions */

/**
 * Returns the height of the given node.
 * NULL is -1
 */
static int Height(Node n)
{
	if (n == NULL)
		return -1;
	return n->height;
}

/**
 * Returns the maximum of two integers
 */
static int max(int a, int b)
{
	return (a > b) ? a : b;
}
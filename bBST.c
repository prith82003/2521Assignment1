
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
static Node GetSmallestNode(Node n);
static bool CheckInsertCase(Node curr, int nKey);
static bool CheckDeleteCase(Node curr, int leftCase);
static void NodeToList(List l, Node curr);

// Write Auxiliary function prototypes here, and declare them as static

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
		fprintf(stderr, "Could not malloc Tree");
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
	if (t->root != NULL)
		FreeNode(t->root);

	free(t);
}

static void FreeNode(Node n)
{
	if (n == NULL)
		return;

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
	if (t->root == NULL)
		return false;

	return NodeSearch(t->root, key);
}

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
	if (TreeSearch(t, key))
	{
		fprintf(stderr, "Value %d already Exists in Tree", key);
		return false;
	}

	if (key == UNDEFINED)
	{
		fprintf(stderr, "Can't Insert Undefined Value");
		return false;
	}

	Node new = NodeCreate(key);

	if (t->root == NULL)
	{
		t->root = new;
		return true;
	}

	t->root = NodeInsert(t->root, new);
	return true;
}

static Node NodeCreate(int k)
{
	Node n = malloc(sizeof(*n));

	if (n == NULL)
	{
		fprintf(stderr, "Could not malloc Node");
		exit(EXIT_FAILURE);
	}

	n->key = k;
	n->left = NULL;
	n->right = NULL;
	n->height = 0;

	return n;
}

static Node NodeInsert(Node curr, Node n)
{
	if (curr == NULL)
		return n;

	if (curr->key > n->key)
		curr->left = NodeInsert(curr->left, n);
	else
		curr->right = NodeInsert(curr->right, n);

	return BalanceTree(curr, n, CheckInsertCase);
}

static Node BalanceTree(Node curr, Node n, bool (*CheckCase)(Node, int))
{
	int balanceFactor = GetBalance(curr);
	bool balanced = (abs(balanceFactor) <= 1);

	if (!balanced)
	{
		Node x = NULL;
		Node y = NULL;
		Node z = curr;

		if (balanceFactor > 1)
		{
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
		}
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

	curr->height = 1 + max(Height(curr->left), Height(curr->right));
	return curr;
}

static int GetBalance(Node n)
{
	return Height(n->left) - Height(n->right);
}

static Node LeftLeftCase(Node x, Node y, Node z)
{
	z->left = y->right;
	y->right = z;

	UpdateHeight(z);
	UpdateHeight(y);

	return y;
}

static Node LeftRightCase(Node x, Node y, Node z)
{
	z->left = x->right;
	x->right = z;
	y->right = x->left;
	x->left = y;

	UpdateHeight(z);
	UpdateHeight(y);
	UpdateHeight(x);

	return x;
}

static Node RightRightCase(Node x, Node y, Node z)
{
	z->right = y->left;
	y->left = z;

	UpdateHeight(z);
	UpdateHeight(y);
	return y;
}

static Node RightLeftCase(Node x, Node y, Node z)
{
	y->left = x->right;
	x->right = y;
	z->right = x->left;
	x->left = z;

	UpdateHeight(z);
	UpdateHeight(y);
	UpdateHeight(x);

	return x;
}

static bool CheckInsertCase(Node curr, int nKey)
{
	return nKey < curr->key;
}

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
	if (key == UNDEFINED)
	{
		fprintf(stderr, "Can't accept UNDEFINED as input");
		return false;
	}

	if (!TreeSearch(t, key))
	{
		fprintf(stderr, "Value to Delete not in Tree");
		return false;
	}

	t->root = NodeDelete(t->root, key);
	return true;
}

static Node NodeDelete(Node curr, int key)
{
	if (curr == NULL)
		return NULL;

	if (key > curr->key)
		curr->right = NodeDelete(curr->right, key);
	else if (key < curr->key)
		curr->left = NodeDelete(curr->left, key);
	else
		return GetNextNode(curr);

	return BalanceTree(curr, NULL, CheckDeleteCase);
}

static Node GetNextNode(Node n)
{
	// One/Zero Child Cases
	if (n->right == NULL)
	{
		Node tempLeft = n->left;
		free(n);
		return tempLeft;
	}
	else if (n->left == NULL)
	{
		Node tempRight = n->right;
		free(n);
		return tempRight;
	}

	// Both Child Case
	Node min = GetSmallestNode(n->right);
	n->key = min->key;

	n->right = NodeDelete(n->right, min->key);
	return n;
}

static bool CheckDeleteCase(Node sub, int leftCase)
{
	int balanceSub = GetBalance(sub);
	return (leftCase) ? balanceSub < 0 : balanceSub <= 0;
}

//! Replace with TreeKthSmallest Once Implemented
static Node GetSmallestNode(Node n)
{
	Node curr = n;

	while (curr != NULL && curr->left != NULL)
		curr = curr->left;

	return curr;
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
	NodeToList(l, t->root);
	return l;
}

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
	// TODO: Complete this function
	return UNDEFINED;
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
	// TODO: Complete this function
	return UNDEFINED;
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
	// TODO: Complete this function
	return UNDEFINED;
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the largest key less than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */
int TreeFloor(Tree t, int key)
{
	// TODO: Complete this function
	return UNDEFINED;
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the smallest key greater than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */
int TreeCeiling(Tree t, int key)
{
	// TODO: Complete this function
	return UNDEFINED;
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
	// TODO: Complete this function
	return ListNew();
}

////////////////////////////////////////////////////////////////////////

/* Helper Functions */

static int Height(Node n)
{
	if (n == NULL)
		return -1;
	return n->height;
}

static int max(int a, int b)
{
	return (a > b) ? a : b;
}
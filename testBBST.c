#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "bBST.h"

typedef struct balance
{
	int height;
	bool balanced;
	Node unbalancedNode;
} Balance;

typedef char *String;
static bool getCommand(String buf);
static String *tokenize(String s, int *ntokens, String seperators);
static void freeTokens(char **tokens);

static void printCurrentLevel(Node root, int level, bool full, FILE *fp);

static void runInsert(Tree t, int argc, char **argv);
static void runInsertRange(Tree t, int argc, char **argv);
static void runInsertRandom(Tree t, int argc, char **argv);
static void runInsertNormal(Tree t, int argc, char **argv);

static void runDelete(Tree t, int argc, char **argv);
static void runPrint(Tree t, int argc, char **argv);
static void executePrint(Tree t, FILE *fp);
static void runQuit(Tree t, int argc, char **argv);
static void runSearch(Tree t, int argc, char **argv);
static void InOrderDetailedPrint(Node n, int parent);
static void InOrderNewPrint(Node n, int maxTab, int trueHeight, char ***InOrderString, int *counter);
static void runClear(Tree t, int argc, char **argv);
static void runHelp();
static void runSave(Tree t, int argc, char **argv);
static void runLoad(Tree t, int argc, char **argv);
static void runCheckBalanced(Tree t, int argc, char **argv);
static Balance TreeCheckBalanced(Node n);
static bool FileExists(String filename);
static int max(int a, int b);
static int height(struct node *node);
static void runKthSmallest(Tree t, int argc, char **argv);
static void runLCA(Tree t, int argc, char **argv);
static void runToList(Tree t, int argc, char **argv);
static void runFloor(Tree t, int argc, char **argv);
static void runCeiling(Tree t, int argc, char **argv);
static void runTests(Tree t, int argc, char **argv);
static void runSearchBetween(Tree t, int argc, char **argv);
static void runInsertTests(Tree t, bool output);
static void runDeleteTests(Tree t, bool output);
static void runBalanceTests(Tree t, bool output);
static void runKthSmallestTests(Tree t, bool output);
static void runKthLargestTests(Tree t, bool output);
static int orderIncreasing(const void *a, const void *b);
static int orderDecreasing(const void *a, const void *b);
static int numNodes(Tree t);
static int NodeCount(Node n);
static void runClearTree(Tree t, int argc, char **argv);
static void executeTests(Tree t, int numTimes, bool output);
static void createDotFile(Node curr, FILE *fp, int *nullCounter);

static int GetHeight(Node n);

#define CHAR_ROW_MAX 100
#define CHAR_COL_MAX 100
#define MAX 8192
#define PRINT_STRING_SIZE 5

typedef struct command
{
	String name;
	void (*func)(Tree, int, char **);
	String modifiers;
	String help;
} Command;

typedef struct size
{
	int width;
	int height;
} Size;

// Scan through file, each command has one letter associated to it, add that letter and its corresponding function and a fitting help message to the Commands array
static Command Commands[] = {
	{"+,add", runInsert, "-[r, R] ", "Insert a new node into the tree"},
	{"-,d,remove", runDelete, "", "Delete a node from the tree"},
	{"p,print", runPrint, "-[l, L, i] ",
	 "Print the tree"},
	{"q,quit,exit", runQuit, "", "Quit the program"},
	{"s,search", runSearch, "", "Search for a node in the tree"},
	{"c,clear", runClear, "", "Clear the tree"},
	{"w,save,write", runSave, "", "Save the tree to a file"},
	{"l,load", runLoad, "", "Load a tree from a file"},
	{"b,balance", runCheckBalanced, "-h ", "Check if the tree is balanced"},
	{"k,kSmallest", runKthSmallest, "", "Find the kth Smallest Element in the Tree"},
	{"a,LCA", runLCA, "", "Find the Lowest Common Ancestor of two nodes in the tree"},
	{"f,floor", runFloor, "", "Find the floor of a node in the tree"},
	{"t,list", runToList, "", "Convert the tree to a list"},
	{"C,ceiling", runCeiling, "", "Find the ceiling of a node in the tree"},
	{"test", runTests, "-[b, i, d, k, K] ", "Run tests on Balance, floor, ceiling"},
	{"delete", runClearTree, "", "Clears Tree"},
	{"sb", runSearchBetween, "", "Search Between Upper and Lower Value"},
	{NULL, NULL, NULL, NULL}};

/**
 * Prints the BST in Level Order
 * From: https://www.geeksforgeeks.org/level-order-tree-traversal/
 */

static void PrintLevelOrder(Node root, bool full, FILE *fp)
{
	int h = height(root);
	int i;
	for (i = 1; i <= h; i++)
		printCurrentLevel(root, i, full, fp);
	fprintf(fp, "\n");
}

/* Print nodes at a current level */
static void printCurrentLevel(Node root, int level, bool full, FILE *fp)
{
	if (root == NULL)
		return;
	if (level == 1)
	{
		if (!full)
			fprintf(fp, "%d ", root->key);
		else
			fprintf(fp, "{\n\tElement: %d\n\tLevel: %d\n}\n", root->key, root->height);
	}
	else if (level > 1)
	{
		printCurrentLevel(root->left, level - 1, full, fp);
		printCurrentLevel(root->right, level - 1, full, fp);
	}
}

static void InOrderPrint(Node n, int maxTab, int trueHeight)
{
	if (n == NULL)
		return;

	InOrderPrint(n->left, maxTab, trueHeight + 1);

	for (int i = 0; i < maxTab - trueHeight; i++)
		printf("\t");

	printf("%d\n", n->key);
	// printf("(%d, %d, %d)\n", *counter, trueHeight, n->height);
	// *counter += 1;

	InOrderPrint(n->right, maxTab, trueHeight + 1);
}

static void InOrderNewPrint(Node n, int maxTab, int trueHeight, char ***InOrderString, int *counter)
{
	if (n == NULL)
		return;

	InOrderNewPrint(n->left, maxTab, trueHeight + 1, InOrderString, counter);

	String key = malloc(sizeof(char) * 10);
	sprintf(key, "%d", n->key);

	char *str = InOrderString[trueHeight][*counter];
	int totalPad = PRINT_STRING_SIZE - strlen(key);

	if (totalPad > 0)
	{
		char *startPos = &str[totalPad / 2];
		snprintf(startPos, strlen(key) + 1, "%s", key);
		for (int i = 0; i < totalPad / 2; i++)
			strncat(startPos + strlen(key), " ", totalPad - (totalPad / 2));
	}

	*counter += 1;
	free(key);
	InOrderNewPrint(n->right, maxTab, trueHeight + 1, InOrderString, counter);
}

static void InOrderDetailedPrint(Node n, int parent)
{
	if (n == NULL)
		return;

	InOrderDetailedPrint(n->left, n->key);

	printf("{\n\tElement: %d\n\tLevel: %d\n\tParent: %d\n}\n", n->key, n->height, parent);

	InOrderDetailedPrint(n->right, n->key);
}

int main(void)
{
	Tree t = TreeNew();
	String buf = malloc(sizeof(char) * MAX + 1);
	memset(buf, '\0', MAX + 1);

	while (getCommand(buf))
	{
		int ntokens = 0;
		char **tokens = tokenize(buf, &ntokens, " \t\r\n");
		if (ntokens == 0)
		{
			free(tokens);
			continue;
		}

		if (strcmp(tokens[0], "q") == 0)
			break;
		else if (strcmp(tokens[0], "?") == 0)
			runHelp();
		else
		{
			bool found = false;
			for (int i = 0; Commands[i].name != NULL; i++)
			{
				int nnames = 0;
				String *names = tokenize(Commands[i].name, &nnames, ",");

				for (int j = 0; j < nnames; j++)
				{
					if (strcmp(tokens[0], names[j]) == 0)
					{
						Commands[i].func(t, ntokens, tokens);
						found = true;
						break;
					}
				}

				freeTokens(names);
			}
			if (!found)
				printf("Unknown command: %s\n", tokens[0]);
		}

		freeTokens(tokens);
	}

	free(buf);
	runQuit(t, 0, NULL);
}

static bool getCommand(String buf)
{
	printf("> ");
	if (fgets(buf, MAX, stdin) != NULL)
	{
		int len = strlen(buf);
		if (len > 0 && buf[len - 1] != '\n')
		{
			buf[len] = '\n';
			buf[len + 1] = '\0';
		}
		return true;
	}
	else
	{
		return false;
	}
}

static char **tokenize(String s, int *ntokens, String separators)
{
	*ntokens = 0;

	char **tokens = calloc((strlen(s) + 1), sizeof(*tokens));
	assert(tokens != NULL);

	while (*s != '\0')
	{
		s += strspn(s, separators);

		if (*s == '\0')
		{
			break;
		}

		size_t length = strcspn(s, separators);

		String token = strndup(s, length);
		assert(token != NULL);
		s += length;

		tokens[*ntokens] = token;
		(*ntokens)++;
	}

	tokens[*ntokens] = NULL;

	tokens = realloc(tokens, (*ntokens + 1) * sizeof(*tokens));
	assert(tokens != NULL);

	return tokens;
}

static void freeTokens(char **tokens)
{
	for (int i = 0; tokens[i] != NULL; i++)
	{
		free(tokens[i]);
	}
	free(tokens);
}

static void runInsert(Tree t, int argc, char **argv)
{
	if (argc > 1 && argv[1][0] == '-')
	{
		switch (argv[1][1])
		{
		case 'r':
			runInsertRandom(t, argc, argv);
			break;
		case 'R':
			runInsertRange(t, argc, argv);
			break;
		default:
			printf("Unknown option: %s\n", argv[1]);
			break;
		}
	}
	else
	{
		runInsertNormal(t, argc, argv);
	}
}

static void runInsertNormal(Tree t, int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		if (!TreeInsert(t, atoi(argv[i])))
			return;
	}
}

static void runInsertRandom(Tree t, int argc, char **argv)
{
	int n = 10;
	if (argc > 2)
		n = atoi(argv[2]);

	for (int i = 0; i < n; i++)
	{
		int r = rand() % 100;
		if (!TreeInsert(t, r))
			return;
	}
}

static void runInsertRange(Tree t, int argc, char **argv)
{
	if (argc < 4)
	{
		printf("Usage: i -R <start> <end>\n");
		return;
	}

	int start = atoi(argv[2]);
	int end = atoi(argv[3]);

	if (end <= start)
	{
		printf("End must be greater than start\n");
		return;
	}

	for (int i = start; i <= end; i++)
	{
		if (!TreeInsert(t, i))
			return;
	}
}

static void runHelp()
{
	for (int i = 0; Commands[i].help != NULL; i++)
		printf("%s:\t%s%s\n", Commands[i].name, Commands[i].modifiers, Commands[i].help);
}

static void runDelete(Tree t, int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		if (!TreeDelete(t, atoi(argv[i])))
			return;
	}
}

static void runPrint(Tree t, int argc, char **argv)
{
	printf("Running Print:\n");
	FILE *fp;
	int nullCounter = 0;
	if (argc > 1 && argv[1][0] == '-')
	{
		switch (argv[1][1])
		{
		case 'l':
			PrintLevelOrder(t->root, false, stdout);
			break;
		case 'L':
			PrintLevelOrder(t->root, true, stdout);
			break;
		case 'i':
			InOrderDetailedPrint(t->root, -1);
			printf("Num Nodes: %d\n", numNodes(t));
			break;
		case 'I':
			InOrderPrint(t->root, (t->root == NULL) ? 0 : t->root->height, 0);
			break;
		case 'f':
			fp = fopen("tree.bst", "w");
			if (fp == NULL)
				printf("Error opening file\n");
			else
				executePrint(t, fp);
			break;
		case 'g':
			fp = fopen("tree.dot", "w");
			if (fp == NULL)
				printf("Error opening file\n");
			else
				createDotFile(t->root, fp, &nullCounter);
			fclose(fp);
			break;
		default:
			printf("Unknown option: %s\n", argv[1]);
		}

		return;
	}

	executePrint(t, stdout);
}

static void executePrint(Tree t, FILE *fp)
{
	Size size;
	size.height = (t->root == NULL) ? 0 : t->root->height + 1;
	size.width = 2 * (size.height * size.height) - 1;

	int counter = 0;
	int maxHeight = (t->root == NULL) ? 0 : t->root->height;

	char ***InOrderString = malloc(sizeof(char **) * size.height);
	for (int i = 0; i < size.height; i++)
	{
		InOrderString[i] = malloc(sizeof(char *) * size.width);
		for (int j = 0; j < size.width; j++)
		{
			InOrderString[i][j] = malloc(sizeof(char) * 10);
			strcpy(InOrderString[i][j], "     ");
		}
	}

	InOrderNewPrint(t->root, maxHeight, 0, InOrderString, &counter);

	for (int i = 0; i < size.height; i++)
	{
		for (int j = 0; j < size.width; j++)
		{
			fprintf(fp, "%s", InOrderString[i][j]);
			free(InOrderString[i][j]);
		}
		fprintf(fp, "\n");
		free(InOrderString[i]);
	}
	free(InOrderString);
}

static void createDotFile(Node curr, FILE *fp, int *nullCounter)
{
	if (curr == NULL)
		return;

	createDotFile(curr->left, fp, nullCounter);

	if (curr->left == NULL)
	{
		(*nullCounter)++;
		fprintf(fp, "null%d [shape=point];\n", *nullCounter);
		fprintf(fp, "%d -> null%d;\n", curr->key, *nullCounter);
	}
	else
		fprintf(fp, "%d -> %d;\n", curr->key, curr->left->key);

	if (curr->right == NULL)
	{
		(*nullCounter)++;
		fprintf(fp, "null%d [shape=point];\n", *nullCounter);
		fprintf(fp, "%d -> null%d;\n", curr->key, *nullCounter);
	}
	else
		fprintf(fp, "%d -> %d;\n", curr->key, curr->right->key);

	createDotFile(curr->right, fp, nullCounter);
}

static void runSearch(Tree t, int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		int num = atoi(argv[i]);
		bool search = TreeSearch(t, num);
		printf("Element %d was %s.\n", num, (search) ? "found" : "not found");
	}
}

static void runClear(Tree t, int argc, char **argv)
{
	system("clear");
}

static void runSave(Tree t, int argc, char **argv)
{
	FILE *fp = fopen("data.bst", "w");
	PrintLevelOrder(t->root, false, fp);
	fclose(fp);
}

static void runLoad(Tree t, int argc, char **argv)
{
	if (!FileExists("data.bst"))
	{
		printf("No Save File Exists\n");
		return;
	}

	runClearTree(t, 0, NULL);

	FILE *fp = fopen("data.bst", "r");

	int elem = 0;
	while (fscanf(fp, " %d", &elem) != -1)
		TreeInsert(t, elem);

	fclose(fp);
}

static void runCheckBalanced(Tree t, int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-h") == 0)
		printf("Height: %d\n", t->root->height);

	Balance balance = TreeCheckBalanced(t->root);
	printf("Tree is %sbalanced.\n", (balance.balanced) ? "" : "not ");
	if (!balance.balanced)
	{
		printf("Unbalanced node: %d\n", balance.unbalancedNode->key);
		printf("Left Height: %d\nRight Height: %d\n", GetHeight(balance.unbalancedNode->left), GetHeight(balance.unbalancedNode->right));
	}
}

static Balance TreeCheckBalanced(Node n)
{
	if (n == NULL)
		return (Balance){-1, true};

	Balance left = TreeCheckBalanced(n->left);
	Balance right = TreeCheckBalanced(n->right);

	if (!left.balanced || !right.balanced)
		return (!right.balanced) ? right : left;

	if (abs(left.height - right.height) > 1)
	{
		Balance b = {1 + max(left.height, right.height), false};
		b.unbalancedNode = n;
		return b;
	}

	return (Balance){
		max(left.height, right.height) + 1, true};
}

static void runKthSmallest(Tree t, int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage: <k>\n");
		return;
	}

	int k = atoi(argv[1]);
	int kth = TreeKthSmallest(t, k);
	printf("The %dth smallest element is ", k);
	if (kth == UNDEFINED)
		printf("Error!\n");
	else
		printf("%d.\n", kth);
}

static void runLCA(Tree t, int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: <a> <b>\n");
		return;
	}

	int a = atoi(argv[1]);
	int b = atoi(argv[2]);
	int lca = TreeLCA(t, a, b);
	printf("The LCA of %d and %d is ", a, b);
	if (lca == UNDEFINED)
		printf("Error!\n");
	else
		printf("%d.\n", lca);
}

static void runFloor(Tree t, int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage: <x>\n");
		return;
	}

	for (int i = 1; i < argc; i++)
	{
		int x = atoi(argv[i]);
		int floor = TreeFloor(t, x);
		printf("The floor of %d is ", x);
		if (floor == UNDEFINED)
			printf("Error!\n");
		else
			printf("%d.\n", floor);
	}
}

static void runCeiling(Tree t, int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage: <x>\n");
		return;
	}

	for (int i = 1; i < argc; i++)
	{
		int x = atoi(argv[i]);
		int ceiling = TreeCeiling(t, x);
		printf("The ceiling of %d is ", x);
		if (ceiling == UNDEFINED)
			printf("Error!\n");
		else
			printf("%d.\n", ceiling);
	}
}

static void runToList(Tree t, int argc, char **argv)
{
	List l = TreeToList(t);
	ListShow(l);
	printf("\n");
	ListFree(l);
}

static void runTests(Tree t, int argc, char **argv)
{
	int numTimes = 1;
	if (argc > 1 && argv[1][0] == '-')
	{
		switch (argv[1][1])
		{
		case 'b':
			runBalanceTests(t, true);
			break;
		case 'i':
			runInsertTests(t, true);
			break;
		case 'd':
			runDeleteTests(t, true);
			break;
		case 'k':
			runKthSmallestTests(t, true);
			break;
		case 'K':
			runKthLargestTests(t, true);
			break;
		case 'q':
			if (argc > 2)
				numTimes = atoi(argv[2]);

			executeTests(t, numTimes, false);
			break;
		default:
			printf("Invalid test type.\n");
			break;
		}

		return;
	}

	if (argc > 1)
		numTimes = atoi(argv[1]);

	executeTests(t, numTimes, true);
}

static void executeTests(Tree t, int numTimes, bool output)
{
	for (int i = 0; i < numTimes; i++)
	{
		runInsertTests(t, output);
		runDeleteTests(t, output);
		runBalanceTests(t, output);
		runKthLargestTests(t, output);
		runKthSmallestTests(t, output);
	}
}

static void runBalanceTests(Tree t, bool output)
{
	runClearTree(t, 0, NULL);
	for (int X = 1; X <= 2500; X++)
	{
		srand(time(NULL));
		int bstSize = rand() % 100 + 1;
		int numbers[bstSize];
		int *removed = calloc(bstSize, sizeof(int));

		for (int i = 0; i < bstSize; i++)
		{
			int num = rand() % 5000 + 1;

			while (TreeSearch(t, num))
				num = rand() % 5000 + 1;

			TreeInsert(t, num);
			numbers[i] = num;
			Balance balance = TreeCheckBalanced(t->root);
			if (!balance.balanced)
			{
				runSave(t, 0, NULL);
				runPrint(t, 0, NULL);
				printf("Unbalanced node: %d\n", balance.unbalancedNode->key);
				printf("Left Height: %d\nRight Height: %d\n", GetHeight(balance.unbalancedNode->left), GetHeight(balance.unbalancedNode->right));
				free(removed);
				return;
			}
		}

		for (int i = 0; i < bstSize; i++)
		{
			int index = rand() % bstSize;

			while (removed[index] == 1)
				index = rand() % bstSize;

			removed[index] = 1;

			TreeDelete(t, numbers[index]);
			Balance balance = TreeCheckBalanced(t->root);
			if (!balance.balanced)
			{
				runSave(t, 0, NULL);
				runPrint(t, 0, NULL);
				printf("Unbalanced node: %d\n", balance.unbalancedNode->key);
				printf("Left Height: %d\nRight Height: %d\n", GetHeight(balance.unbalancedNode->left), GetHeight(balance.unbalancedNode->right));
				free(removed);
				return;
			}
		}

		free(removed);
		if (output)
			printf("Succesful Balance Run %d!\n", X);
	}
}

static void runInsertTests(Tree t, bool output)
{
	runClearTree(t, 0, NULL);
	for (int X = 1; X <= 2500; X++)
	{
		srand(time(NULL));
		int bstSize = rand() % 100 + 1;

		for (int i = 0; i < bstSize; i++)
		{
			int num = rand() % 5000 + 1;

			while (TreeSearch(t, num))
				num = rand() % 5000 + 1;

			TreeInsert(t, num);
			if (!TreeSearch(t, num))
			{
				runSave(t, 0, NULL);
				runPrint(t, 0, NULL);
				printf("Failed to insert %d.\n", num);
				return;
			}
		}

		if (output)
			printf("Succesful Insert Run %d!\n", X);
		runClearTree(t, 0, NULL);
	}
}

static void runDeleteTests(Tree t, bool output)
{
	runClearTree(t, 0, NULL);
	for (int X = 1; X <= 2500; X++)
	{
		srand(time(NULL));
		int bstSize = rand() % 100 + 1;
		int numbers[bstSize];
		int *removed = calloc(bstSize, sizeof(int));

		for (int i = 0; i < bstSize; i++)
		{
			int num = rand() % 5000 + 1;

			while (TreeSearch(t, num))
				num = rand() % 5000 + 1;

			TreeInsert(t, num);
			numbers[i] = num;
		}

		for (int i = 0; i < bstSize; i++)
		{
			int index = rand() % bstSize;

			while (removed[index] == 1)
				index = rand() % bstSize;

			removed[index] = 1;

			TreeDelete(t, numbers[index]);
			if (TreeSearch(t, numbers[index]))
			{
				runSave(t, 0, NULL);
				runPrint(t, 0, NULL);
				printf("Failed to delete %d.\n", numbers[index]);
				free(removed);
				return;
			}
		}

		free(removed);
		if (output)
			printf("Succesful Delete Run %d!\n", X);
	}
}

static void runKthSmallestTests(Tree t, bool output)
{
	runClearTree(t, 0, NULL);
	for (int X = 1; X <= 2500; X++)
	{
		srand(time(NULL));
		int bstSize = rand() % 250 + 1;
		int numbers[bstSize];

		for (int i = 0; i < bstSize; i++)
		{
			int num = rand() % 5000 + 1;

			while (TreeSearch(t, num))
				num = rand() % 5000 + 1;

			TreeInsert(t, num);
			numbers[i] = num;
		}

		qsort(numbers, bstSize, sizeof(int), orderIncreasing);

		for (int i = 0; i < bstSize; i++)
		{
			int num = TreeKthSmallest(t, i + 1);
			if (num != numbers[i])
			{
				runSave(t, 0, NULL);
				runPrint(t, 0, NULL);
				printf("Failed to find %dth smallest number. Expected %d, got %d.\n", i + 1, numbers[i], num);
				return;
			}
		}

		runClearTree(t, 0, NULL);

		if (output)
			printf("Succesful kTh Smallest Run %d!\n", X);
	}
}

static void runKthLargestTests(Tree t, bool output)
{
	runClearTree(t, 0, NULL);
	for (int X = 1; X <= 2500; X++)
	{
		srand(time(NULL));
		int bstSize = rand() % 250 + 1;
		int numbers[bstSize];

		for (int i = 0; i < bstSize; i++)
		{
			int num = rand() % 5000 + 1;

			while (TreeSearch(t, num))
				num = rand() % 5000 + 1;

			TreeInsert(t, num);
			numbers[i] = num;
		}

		qsort(numbers, bstSize, sizeof(int), orderDecreasing);

		for (int i = 0; i < bstSize; i++)
		{
			int num = TreeKthLargest(t, i + 1);
			if (num != numbers[i])
			{
				runSave(t, 0, NULL);
				runPrint(t, 0, NULL);
				printf("Failed to find %dth largest number. Expected %d, got %d.\n", i + 1, numbers[i], num);
				return;
			}
		}

		runClearTree(t, 0, NULL);

		if (output)
			printf("Succesful kTh Largest Run %d!\n", X);
	}
}

static int orderIncreasing(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}

static int orderDecreasing(const void *a, const void *b)
{
	return (*(int *)b - *(int *)a);
}

static int numNodes(Tree t)
{
	if (t->root == NULL)
		return 0;

	return NodeCount(t->root);
}

static int NodeCount(Node t)
{
	if (t == NULL)
		return 0;

	return 1 + NodeCount(t->left) + NodeCount(t->right);
}

static void runClearTree(Tree t, int argc, char **argv)
{
	while (t->root != NULL)
		TreeDelete(t, t->root->key);
}

static void runSearchBetween(Tree t, int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: <min> <max>\n");
		return;
	}

	int min = atoi(argv[1]);
	int max = atoi(argv[2]);
	List l = TreeSearchBetween(t, min, max);
	printf("Search Between %d and %d: ", min, max);
	ListShow(l);
	ListFree(l);
	printf("\n");
}

static void runQuit(Tree t, int argc, char **argv)
{
	TreeFree(t);
	exit(EXIT_SUCCESS);
}

/* Helper Functions */

static bool FileExists(String filename)
{
	struct stat buffer;
	return (stat(filename, &buffer) == 0);
}

static int max(int a, int b)
{
	return (a > b) ? a : b;
}

/* For Level Order Print from GeeksforGeeks */
static int height(struct node *node)
{
	if (node == NULL)
		return 0;
	else
	{
		/* compute the height of each subtree */
		int lheight = height(node->left);
		int rheight = height(node->right);

		/* use the larger one */
		if (lheight > rheight)
			return (lheight + 1);
		else
			return (rheight + 1);
	}
}

static int GetHeight(Node n)
{
	if (n == NULL)
		return -1;
	return n->height;
}
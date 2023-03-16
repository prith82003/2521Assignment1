// Implementation of the Queue ADT using a linked list

// !!! DO NOT MODIFY THIS FILE !!!

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>

#include "Queue.h"
#include "bBST.h"

typedef struct element *Element;
struct element
{
	Item item;
	Element next;
};

struct queue
{
	Element head;
	Element tail;
	int size;
};

static Element newNode(Item it);

static void runInsert(Tree t);
static void runDelete(Tree t);
static void runPrint(Tree t, char buf);
static void runQuit(Tree t);
static void runSearch(Tree t);
static void InOrderDetailedPrint(Node n, int parent);
static void runClear();
static void runHelp();
static void runSave(Tree t);
static Tree runLoad(Tree t);
static bool FileExists(char *filename);

/**
 * Creates a new empty queue
 */
Queue QueueNew(void)
{
	Queue q = malloc(sizeof(*q));
	if (q == NULL)
	{
		fprintf(stderr, "couldn't allocate Queue\n");
		exit(EXIT_FAILURE);
	}

	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
	return q;
}

/**
 * Frees all resources associated with the given queue
 */
void QueueFree(Queue q)
{
	Element curr = q->head;
	while (curr != NULL)
	{
		Element temp = curr;
		curr = curr->next;
		free(temp);
	}
	free(q);
}

/**
 * Adds an item to the end of the queue
 */
void QueueEnqueue(Queue q, Item it)
{
	Element n = newNode(it);
	if (q->size == 0)
	{
		q->head = n;
	}
	else
	{
		q->tail->next = n;
	}
	q->tail = n;
	q->size++;
}

static Element newNode(Item it)
{
	Element n = malloc(sizeof(*n));
	if (n == NULL)
	{
		fprintf(stderr, "couldn't allocate Node\n");
		exit(EXIT_FAILURE);
	}

	n->item = it;
	n->next = NULL;
	return n;
}

/**
 * Removes an item from the front of the queue and returns it
 * Assumes that the queue is not empty
 */
Item QueueDequeue(Queue q)
{
	assert(q->size > 0);

	Element newHead = q->head->next;
	Item it = q->head->item;
	free(q->head);
	q->head = newHead;
	if (newHead == NULL)
	{
		q->tail = NULL;
	}
	q->size--;
	return it;
}

/**
 * Gets the item at the front of the queue without removing it
 * Assumes that the queue is not empty
 */
Item QueueFront(Queue q)
{
	assert(q->size > 0);

	return q->head->item;
}

/**
 * Gets the size of the given queue
 */
int QueueSize(Queue q)
{
	return q->size;
}

/**
 * Returns true if the queue is empty, and false otherwise
 */
bool QueueIsEmpty(Queue q)
{
	return q->size == 0;
}

/**
 * Prints the queue to the given file with items space-separated
 */
void QueueDump(Queue q, FILE *fp)
{
	for (Element curr = q->head; curr != NULL; curr = curr->next)
	{
		fprintf(fp, "%p ", curr->item);
	}
	fprintf(fp, "\n");
}

void BSTreeLevelOrder(Node t, bool full, FILE *fp)
{
	if (t == NULL)
		return;

	Queue q = QueueNew();
	QueueEnqueue(q, t);
	while (!QueueIsEmpty(q))
	{
		Node item = QueueDequeue(q);

		if (full)
			fprintf(fp, "{\n\tElement: %d\n\tLevel: %d\n}\n", item->key, item->height);
		else
			fprintf(fp, "%d ", item->key);

		if (item->left != NULL)
			QueueEnqueue(q, item->left);
		if (item->right != NULL)
			QueueEnqueue(q, item->right);
	}

	printf("\n");
	QueueFree(q);
}

static void InOrderPrint(Node n, int maxTab, int trueHeight)
{
	if (n == NULL)
		return;

	InOrderPrint(n->left, maxTab, trueHeight + 1);

	for (int i = 0; i < maxTab - trueHeight; i++)
		printf("\t");

	printf("%d\n", n->key);
	InOrderPrint(n->right, maxTab, trueHeight + 1);
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

	while (1)
	{
		char command = '\0';
		char buf = '\0';
		printf("> ");
		scanf(" %c%c", &command, &buf);

		switch (command)
		{
		case '+':
			runInsert(t);
			break;
		case '-':
			runDelete(t);
			break;
		case 'p':
			runPrint(t, buf);
			break;
		case 'q':
			runQuit(t);
		case 's':
			runSearch(t);
			break;
		case 'c':
			runClear();
			break;
		case '?':
			runHelp();
			break;
		case 'w':
			runSave(t);
			break;
		case 'l':
			t = runLoad(t);
			break;
		default:
			printf("Invalid Input: %c. Buffer: %c.\n", command, buf);
		}
	}

	runQuit(t);
}

static void runInsert(Tree t)
{
	char buffer = '\0';
	int digit = 0;

	printf("Run Insert\n");

	do
	{
		scanf(" %d%c", &digit, &buffer);
		if (!TreeInsert(t, digit))
			return;
	} while (buffer != '\n');
}

static void runHelp()
{
	printf("+: Add elements to tree\n-: Delete Element from Tree\np: Print Tree, -l for level order, -L for detailed level order, -v for detailed in order\ns: Search for element\nc: Clear stdout\nw: Save Current BST to File\nl: Load BST from file if it exists\n?: Help\n");
}

static void runDelete(Tree t)
{
	char buffer = '\0';
	int digit = 0;

	printf("Run Insert\n");

	do
	{
		scanf(" %d%c", &digit, &buffer);
		if (!TreeDelete(t, digit))
			return;
	} while (buffer != '\n');
}

static void runPrint(Tree t, char buf)
{
	// TODO: Adjust Height tabbing. Use max to renormalise tab indents
	// * Use Recursive to get max height
	// * Print using new max height

	if (buf != '\n')
	{
		char option = '0';
		char d = '0';

		scanf(" %c%c", &d, &option);

		switch (option)
		{
		case 'l':
			BSTreeLevelOrder(t->root, false, stdout);
			break;
		case 'L':
			BSTreeLevelOrder(t->root, true, stdout);
			break;
		case 'v':
			InOrderDetailedPrint(t->root, -1);
			break;
		default:
			printf("Incorrect Usage\n");
		}

		return;
	}

	printf("Running Print:\n");
	int maxHeight = (t->root == NULL) ? 0 : t->root->height;
	InOrderPrint(t->root, maxHeight, 0);
}

static void runSearch(Tree t)
{
	int digit = 0;
	char buf = '\0';

	scanf(" %d%c", &digit, &buf);
	bool search = TreeSearch(t, digit);

	printf("Element %d was %s.\n", digit, (search) ? "found" : "not found");
}

static void runClear()
{
	system("clear");
}

static void runSave(Tree t)
{
	FILE *fp = fopen("data.bst", "w");
	BSTreeLevelOrder(t->root, false, fp);
	fclose(fp);
}

static Tree runLoad(Tree t)
{
	if (!FileExists("data.bst"))
	{
		printf("No Save File Exists\n");
		return t;
	}

	TreeFree(t);
	t = NULL;
	t = TreeNew();

	FILE *fp = fopen("data.bst", "r");

	int elem = 0;
	while (fscanf(fp, " %d", &elem) != -1)
		TreeInsert(t, elem);

	fclose(fp);
	return t;
}

static void runQuit(Tree t)
{
	TreeFree(t);
	exit(EXIT_SUCCESS);
}

static bool FileExists(char *filename)
{
	struct stat buffer;
	return (stat(filename, &buffer) == 0);
}
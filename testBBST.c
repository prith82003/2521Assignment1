// Implementation of the Queue ADT using a linked list

// !!! DO NOT MODIFY THIS FILE !!!

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

void BSTreeLevelOrder(Node t)
{
    if (t == NULL)
        return;

    Queue q = QueueNew();
    QueueEnqueue(q, t);
    while (!QueueIsEmpty(q))
    {
        Node item = QueueDequeue(q);
        for (int i = 0; i < item->height; i++)
        {
            printf("\t");
        }
        printf("%d\n", item->key);
        if (item->left != NULL)
            QueueEnqueue(q, item->left);
        if (item->right != NULL)
            QueueEnqueue(q, item->right);
    }

    QueueFree(q);
}

void InOrderPrint(Node n)
{
    if (n == NULL)
        return;

    InOrderPrint(n->left);

    for (int i = 0; i < n->height; i++)
        printf("\t");

    printf("%d\n", n->key);
    InOrderPrint(n->right);
}

int main(void)
{
    Tree t = TreeNew();

    /* Even BST */
    // TreeInsert(t, 20);
    // TreeInsert(t, 14);
    // TreeInsert(t, 35);
    // TreeInsert(t, 16);
    // TreeInsert(t, 15);
    // TreeInsert(t, 30);
    // TreeInsert(t, 36);

    /* Uneven BST */
    TreeInsert(t, 7);
    TreeInsert(t, 6);
    TreeInsert(t, 5);
    TreeInsert(t, 4);
    TreeInsert(t, 3);
    TreeInsert(t, 2);
    TreeInsert(t, 1);

    // BSTreeLevelOrder(t->root);
    InOrderPrint(t->root);
    TreeFree(t);
}
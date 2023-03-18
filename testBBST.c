#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

#include "bBST.h"

typedef char *String;
static bool getCommand(String buf);
static String *tokenize(String s, int *ntokens);
static void freeTokens(char **tokens);

void printCurrentLevel(Node root, int level, bool full, FILE *fp);

static void runInsert(Tree t, int argc, char **argv);
static void runDelete(Tree t, int argc, char **argv);
static void runPrint(Tree t, int argc, char **argv);
static void runQuit(Tree t, int argc, char **argv);
static void runSearch(Tree t, int argc, char **argv);
static void InOrderDetailedPrint(Node n, int parent);
static void InOrderNewPrint(Node n, int maxTab, int trueHeight, char ***InOrderString, int *counter);
static void runClear(Tree t, int argc, char **argv);
static void runHelp();
static void runSave(Tree t, int argc, char **argv);
static void runLoad(Tree t, int argc, char **argv);
static void runCheckBalanced(Tree t, int argc, char **argv);
static int TreeCheckBalanced(Node n);
static bool FileExists(String filename);
static int max(int a, int b);
static int height(struct node *node);

#define CHAR_ROW_MAX 100
#define CHAR_COL_MAX 100
#define MAX 8192
#define NUM_COMMANDS 9

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
static Command Commands[NUM_COMMANDS] = {
    {"+", runInsert, "", "Insert a new node into the tree"},
    {"-", runDelete, "", "Delete a node from the tree"},
    {"p", runPrint, "-[l, L, i] ",
     "Print the tree"},
    {"q", runQuit, "", "Quit the program"},
    {"s", runSearch, "", "Search for a node in the tree"},
    {"c", runClear, "", "Clear the tree"},
    {"w", runSave, "", "Save the tree to a file"},
    {"l", runLoad, "", "Load a tree from a file"},
    {"b", runCheckBalanced, "-h ", "Check if the tree is balanced"}};

/**
 * Prints the BST in Level Order
 * From: https://www.geeksforgeeks.org/level-order-tree-traversal/
 */

void PrintLevelOrder(Node root, bool full, FILE *fp)
{
    int h = height(root);
    int i;
    for (i = 1; i <= h; i++)
        printCurrentLevel(root, i, full, fp);
    fprintf(fp, "\n");
}

/* Print nodes at a current level */
void printCurrentLevel(Node root, int level, bool full, FILE *fp)
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

    strncpy(str, key, strlen(key));

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
        char **tokens = tokenize(buf, &ntokens);
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
            for (int i = 0; i < NUM_COMMANDS; i++)
            {
                if (strcmp(tokens[0], Commands[i].name) == 0)
                {
                    Commands[i].func(t, ntokens, tokens);
                    found = true;
                    break;
                }
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

static char **tokenize(String s, int *ntokens)
{
    String separators = " \t\r\n";
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
    for (int i = 1; i < argc; i++)
    {
        if (!TreeInsert(t, atoi(argv[i])))
            return;
    }
}

static void runHelp()
{
    for (int i = 0; i < NUM_COMMANDS; i++)
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
            break;
        case 'I':
            InOrderPrint(t->root, (t->root == NULL) ? 0 : t->root->height, 0);
            break;
        default:
            printf("Unknown option: %s\n", argv[1]);
        }

        return;
    }

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
            strcpy(InOrderString[i][j], "   ");
        }
    }

    InOrderNewPrint(t->root, maxHeight, 0, InOrderString, &counter);

    for (int i = 0; i < size.height; i++)
    {
        for (int j = 0; j < size.width; j++)
        {
            printf("%s", InOrderString[i][j]);
            free(InOrderString[i][j]);
        }
        printf("\n");
        free(InOrderString[i]);
    }
    free(InOrderString);
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
    }

    while (t->root != NULL)
        TreeDelete(t, t->root->key);

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

    bool balanced = TreeCheckBalanced(t->root);
    printf("Tree is %sbalanced.\n", (balanced) ? "" : "not ");
}

static int TreeCheckBalanced(Node n)
{
    if (n == NULL)
        return -1;

    int left = TreeCheckBalanced(n->left);
    int right = TreeCheckBalanced(n->right);

    if (abs(left - right) > 1)
        return 9999999;

    return 1 + max(left, right);
}

static void runQuit(Tree t, int argc, char **argv)
{
    TreeFree(t);
    exit(EXIT_SUCCESS);
}

static bool FileExists(String filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

static int max(int a, int b)
{
    return (a > b) ? a : b;
}

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
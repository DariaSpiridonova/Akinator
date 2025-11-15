#ifndef AKINATOR
#define AKINATOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>

#define ASSERTS(tree)\
        assert(tree != NULL);\
        assert(tree->num_of_el >= 0);\
        assert(tree->root != NULL);\
        assert(tree->root->parent == NULL);

#define AKINATOR_DUMP(tree)\
        AkinatorDump(tree,__FILE__, __LINE__)

#define BEAUTIFUL_DUMP 
typedef int used_type;

const used_type POIZON = 765418;
const ssize_t SIZE_OF_NAME = 200;
const size_t STR_SIZE = 50;
const size_t ANSWER_SIZE = 5;
const size_t LEN = 5;
const char link_to_graphviz_file[] = "../Graphviz/akinator_";

struct NODE_T
{
    char *data;
    struct NODE_T *left;
    struct NODE_T *right;
    struct NODE_T *parent;
};

typedef struct NODE_T node_t;

typedef struct
{
    ssize_t num_of_el;
    node_t *root;
    const char *file_name;
} binary_tree;

enum Akinator_Errors
{
    NO_ERROR,
    NULL_POINTER_ON_TREE,
    TREE_IS_EMPTY,
    ELEMENT_NOT_FOUND,
    ERROR_DURING_THE_CREATION_OF_THE_TREE,
    NEGATIVE_NUM_OF_ELEMENTS,
    NULL_POINTER_ON_ROOT,
    ROOT_HAVE_PARENT,
    SON_HAS_WRONG_PARENT,
    ERROR_DURING_OPENING_FILE,
    ERROR_DURING_CLOSING_FILE,
    ERROR_DURING_MEMORY_ALLOCATION,
    ERROR_DURING_READ_FILE,
    INCORRECT_DATA_IN_FILE
};

Akinator_Errors AkinatorInit(binary_tree *tree, const char *logfile_name);

Akinator_Errors AkinatorVerify(binary_tree *tree);
bool CheckSonsAndParents(node_t *node, node_t **son, node_t **parent);
void CheckSonsAndParentsRecursive(node_t *node, bool *flag, node_t **son, node_t **parent);

Akinator_Errors AkinatorDestroy(binary_tree *tree);
Akinator_Errors AkinatorDestroyRecursive(binary_tree *tree, node_t **node);

void AkinatorDump(binary_tree *tree, const char *file, int line);
void DumpToConsole(const binary_tree *tree, const char *file, int line, ssize_t *rank);
void DumpToLogfile(const binary_tree *tree, const char *logfile_name, const char *gvfile_name, ssize_t *rank);
void Dump(FILE *fp, const binary_tree *tree, ssize_t *rank);
void ShowTree(FILE *fp, node_t *node, ssize_t *rank, ssize_t *cur_rank);
void PrintEdges(FILE *fp, node_t *node);
void LinkEdges(FILE *fp, node_t *node);
void CreateGraph(const binary_tree *tree, const char *gvfile_name);


Akinator_Errors AkinatorGame(binary_tree *tree);
void GetARequestNumber(ssize_t *num);
Akinator_Errors PlayGame(binary_tree *tree);
void AskQuestion(node_t **node);
Akinator_Errors Victory(binary_tree *tree);
Akinator_Errors AddingNode(binary_tree *tree, node_t *node);
Akinator_Errors NodeInit(binary_tree *tree, node_t **node, node_t *parent, char **string);
bool GetAnswer();
void GetObject(char **ptr);
void GetFeature(char **ptr, char *object, node_t *node);
bool GetDifference(char **ptr, char *object, node_t *node);
void DiscardSuperfluous();

Akinator_Errors FindObject(binary_tree *tree);
bool FindObjectRecursive(binary_tree *tree, node_t *node, char *object, node_t **ptr_object, bool *object_was_found);
void GetDescription(binary_tree *tree, node_t * ptr_object);

Akinator_Errors CompareObjects(binary_tree *tree);

Akinator_Errors SaveTreeToFile(binary_tree *tree, char *name_of_file);
void SaveTreeToFileRecursive(FILE *fp, node_t *node);

Akinator_Errors ReadTreeFromFile(binary_tree *tree, char *name_of_file);
void SplitIntoParts(char *tree_buffer);
char *ReadNodeFromBuffer(binary_tree *tree, char **position, node_t **node, node_t *parent);
Akinator_Errors NodeFromFileInit(binary_tree *tree, char **position, node_t **node, node_t *parent);
size_t return_num_of_bytes_in_file(int fd1);
void SkipSpaces(char **position);

bool OpenFileSuccess(FILE *fp, const char * file_name);
bool CloseFileSuccess(FILE *fp, const char * file_name);

bool PrintError(Akinator_Errors err);

#endif
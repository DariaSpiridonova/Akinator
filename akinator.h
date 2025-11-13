#ifndef AKINATOR
#define AKINATOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>

#define AKINATOR_DUMP(tree)\
        AkinatorDump(tree,__FILE__, __LINE__)

typedef int used_type;

const used_type POIZON = 765418;
const ssize_t SIZE_OF_NAME = 200;
const size_t STR_SIZE = 50;
const size_t ANSWER_SIZE = 5;
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
    SON_HAS_WRONG_PARENT 
};

Akinator_Errors AkinatorInit(binary_tree *tree, const char *logfile_name);

Akinator_Errors AkinatorVerify(binary_tree *tree);
bool check_sons_and_parents(node_t *node, node_t **son, node_t **parent);
void check_sons_and_parents_recursive(node_t *node, bool *flag, node_t **son, node_t **parent);

Akinator_Errors AkinatorDestroy(binary_tree *tree, node_t **node);

void AkinatorDump(binary_tree *tree, const char *file, int line);
void dump_to_console(const binary_tree *tree, const char *file, int line, ssize_t *rank);
void dump_to_logfile(const binary_tree *tree, const char *logfile_name, const char *gvfile_name, ssize_t *rank);
void dump(FILE *fp, const binary_tree *tree, ssize_t *rank);
void show_tree(FILE *fp, node_t *node, ssize_t *rank, ssize_t *cur_rank);
void print_edges(FILE *fp, node_t *node);
void link_edges(FILE *fp, node_t *node);
void create_graph(const binary_tree *tree, const char *gvfile_name);


Akinator_Errors AkinatorGame(binary_tree *tree);
void get_a_request_number(ssize_t *num);
Akinator_Errors play_game(binary_tree *tree);
Akinator_Errors NodeInit(binary_tree *tree, node_t **node, node_t *parent, char **string);
bool start_again(char **answer);
void get_answer(char **answer);
void get_object(char **ptr);
void get_feature(char **ptr, char *object, node_t *node);
void discard_superfluous();

Akinator_Errors find_object(binary_tree *tree);
bool find_object_recursive(binary_tree *tree, node_t *node, char *object, node_t **ptr_object, bool *flag);
void get_description(binary_tree *tree, node_t * ptr_object);

Akinator_Errors compare_objects(binary_tree *tree);

bool open_file_success(FILE *fp, const char * file_name);
bool close_files_success(FILE *fp, const char * file_name);

bool print_error(Akinator_Errors err);

#endif
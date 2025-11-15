#include "akinator.h"

Akinator_Errors AkinatorInit(binary_tree *tree, const char *logfile_name)
{
    assert(tree != NULL);
    assert(logfile_name != NULL);
    
    tree->num_of_el = 1;
    tree->file_name = logfile_name;
    tree->root = (node_t *)calloc(1, sizeof(node_t));
    if (tree->root == NULL)
    {
        return ERROR_DURING_THE_CREATION_OF_THE_TREE;
    }

    tree->root->data = strdup("Nothing");

    tree->root->left = NULL;
    tree->root->right = NULL;
    tree->root->parent = NULL;

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

Akinator_Errors AkinatorVerify(binary_tree *tree)
{
    if (tree == NULL) return NULL_POINTER_ON_TREE;
    else if (tree->num_of_el < 0) return NEGATIVE_NUM_OF_ELEMENTS;
    else if (tree->root == NULL) return NULL_POINTER_ON_ROOT; // не забыть в delete учесть удаление корня
    else if (tree->root->parent != NULL) return ROOT_HAVE_PARENT;  
    
    node_t *parent = NULL;
    node_t *son = NULL;

    if (!CheckSonsAndParents(tree->root, &son, &parent))
    {
        printf("son %p has a parent %p, but not %p", son, son->parent, parent);
        return SON_HAS_WRONG_PARENT;
    } 

    return NO_ERROR;
}

bool CheckSonsAndParents(node_t *node, node_t **son, node_t **parent)
{
    bool flag = true;
    CheckSonsAndParentsRecursive(node, &flag, son, parent);

    return flag;
}

void CheckSonsAndParentsRecursive(node_t *node, bool *flag, node_t **son, node_t **parent)
{
    if (node == NULL)
        return;

    CheckSonsAndParentsRecursive(node->left, flag, son, parent);

    if (node->left != NULL) 
    {
        if (node->left->parent != node) 
        {
            *flag = false;
            *son = node->left->parent;
            *parent = node;
        }
    }
    if (node->right != NULL) 
    {
        if (node->right->parent != node) 
        {
            *flag = false;
            *son = node->right->parent;
            *parent = node;
        }
    }

    CheckSonsAndParentsRecursive(node->right, flag, son, parent);
    
    return;
}

Akinator_Errors AkinatorDestroy(binary_tree *tree)
{
    ASSERTS(tree);

    Akinator_Errors err = NO_ERROR;
    err = AkinatorDestroyRecursive(tree, &(tree->root));
    return err;
}

Akinator_Errors AkinatorDestroyRecursive(binary_tree *tree, node_t **node)
{
    assert(tree != NULL);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
    {
        return err;
    }

    if (*node == NULL)
    {
        printf("node = NULL\n");
        return NO_ERROR;
    }
    AkinatorDestroyRecursive(tree, &((*node)->left));
    AkinatorDestroyRecursive(tree, &((*node)->right));

    printf("%s\n", (*node)->data);
    free((*node)->data);
    (*node)->data = NULL;
    free(*node);
    tree->num_of_el--;
    printf("Free\n");
    *node = NULL;

    return err;
}

void AkinatorDump(binary_tree *tree, const char *file, int line)
{
    ASSERTS(tree);

    ssize_t rank = 0;
    DumpToConsole(tree, file, line, &rank);

    time_t rawtime;      
    struct tm *timeinfo; 
    char buffer[80];

    // Get the current calendar time
    time(&rawtime);

    // Convert the calendar time to local time
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", timeinfo);

    // Print the formatted time string
    printf("Current time: %s\n", buffer);
    struct timespec tstart={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    char gvfile_name[SIZE_OF_NAME] = {0};
    sprintf(gvfile_name, "%s%s%s%ld.gv", link_to_graphviz_file, tree->file_name, buffer, tstart.tv_nsec);
    printf("%s%s%s%ld.gv\n", link_to_graphviz_file, tree->file_name, buffer, tstart.tv_nsec);

    DumpToLogfile(tree, tree->file_name, gvfile_name, &rank);

    CreateGraph(tree, gvfile_name);
}

void DumpToConsole(const binary_tree *tree, const char *file, int line, ssize_t *rank)
{
    printf("ListDump called from %s :%d\n", file, line);
    Dump(stdout, tree, rank);
}

void Dump(FILE *fp, const binary_tree *tree, ssize_t *rank)
{
    fprintf(fp, "ROOT[%p]\n", tree->root);
    fprintf(fp, "{\n");
    fprintf(fp, "    num_of_el = %zd\n", tree->num_of_el);
    fprintf(fp, "    Tree content:\n");
    ssize_t cur_rank = 0;
    ShowTree(fp, tree->root, rank, &cur_rank);
    fprintf(fp, "    rank = %zd\n", *rank);
    fprintf(fp, "\n}");
    fprintf(fp, "\n");
}

void DumpToLogfile(const binary_tree *tree, const char *logfile_name, const char *gvfile_name, ssize_t *rank)
{
    FILE *fp = fopen(logfile_name, "a");

    if (!OpenFileSuccess(fp, logfile_name))
    {
        return;
    }

    fprintf(fp, "<pre>\n");
    Dump(fp, tree, rank);

    fprintf(fp, "<img src=\"%*s.png\" alt=\"Graphviz image\" width=\"1000\">", (int)strlen(gvfile_name) - 3, gvfile_name);

    if (!CloseFileSuccess(fp, logfile_name))
    {
        return;
    }
}

void CreateGraph(const binary_tree *tree, const char *gvfile_name)
{
    FILE *fp = fopen(gvfile_name, "w");

    if (!OpenFileSuccess(fp, gvfile_name))
    {
        return;
    }

    fprintf(fp, "digraph Akinator_game {\n");
    fprintf(fp, "   rankdir=TB;\n");
    fprintf(fp, "   node[shape=\"record\", style=\"filled\", fillcolor=\"Pink\", fontsize=20, pin = true];\n");
    fprintf(fp, "   splines = ortho;\n");
    fprintf(fp, "   rank = same;\n");
    fprintf(fp, "   nodesep = 0.5;\n");
    fprintf(fp, "   edge[arrowsize = 0.5];\n");
    fprintf(fp, "   bgcolor=\"LightBlue\";\n");

    PrintEdges(fp, tree->root);
    #ifdef BEAUTIFUL_DUMP
    fprintf(fp, "\"node_%p\" [fillcolor = \"violet\", label = \"{<data> %s | {<left_answer> yes |<right_answer> no}}\"];\n", tree->root, tree->root->data);
    #else 
    fprintf(fp, "\"node_%p\" [fillcolor = \"violet\", label = \"{<parent> %p | <data> %s | {<left> %p |<right> %p} | {<left_answer> yes |<right_answer> no}}\"];\n", tree->root, tree->root->parent, tree->root->data, tree->root->left, tree->root->right);
    #endif
    LinkEdges(fp, tree->root);

    fprintf(fp, "\n}");
    if (!CloseFileSuccess(fp, gvfile_name))
    {
        return;
    }

    char command[SIZE_OF_NAME * 2] = {0};
    printf("gvfile_name = %s, gvfile_name = %*s\n", gvfile_name, (int)strlen(gvfile_name) - 3, gvfile_name);
    sprintf(command, "dot %s -Tpng -o %*s.png", gvfile_name, (int)strlen(gvfile_name) - 3, gvfile_name);
    
    int error = system(command);
    if (error)
    {
        printf("%d", error);
        //assert(0);
    }
}

void PrintEdges(FILE *fp, node_t *node)
{
    if (node == NULL)
        return;

    PrintEdges(fp, node->left);
    #ifdef BEAUTIFUL_DUMP
    if (node->left == NULL)
         fprintf(fp, "\"node_%p\" [fillcolor = \"Pink\", label = \"{<data> %s}\"];\n", node, node->data);
    else fprintf(fp, "\"node_%p\" [fillcolor = \"Pink\", label = \"{<data> %s | {<left_answer> yes |<right_answer> no}}\"];\n", node, node->data);

    #else
    if (node->left == NULL)
    fprintf(fp, "\"node_%p\" [fillcolor = \"Pink\", label = \"{<parent> %p | <data> %s | {<left> %p |<right> %p}}\"];\n", node, node->parent, node->data, node->left, node->right);
    else fprintf(fp, "\"node_%p\" [fillcolor = \"Pink\", label = \"{<parent> %p | <data> %s | {<left> %p |<right> %p} | {<left_answer> yes |<right_answer> no}}\"];\n", node, node->parent, node->data, node->left, node->right);
    #endif
    PrintEdges(fp, node->right);

    return;
}

void LinkEdges(FILE *fp, node_t *node)
{
    if (node == NULL)
        return;

    LinkEdges(fp, node->left);
    
    if (node->left != NULL) {
        printf("node_left = %p", node->left);
        fprintf(fp, "\"node_%p\":left_answer -> \"node_%p\";\n", node, node->left);}

    if (node->right != NULL)
        fprintf(fp, "\"node_%p\":right_answer -> \"node_%p\";\n", node, node->right);
    
    LinkEdges(fp, node->right);

    return;
}

void ShowTree(FILE *fp, node_t *node, ssize_t *rank, ssize_t *cur_rank)
{
    if (node == NULL)
        return;
    fprintf(fp, "(");
    (*cur_rank)++;
    ShowTree(fp, node->left, rank, cur_rank);
    if (*cur_rank > *rank) *rank = *cur_rank;
    fprintf(fp, "\n        node.parent = %p\n        NODE.DATA = %s\n        node.left = %p\n        node.right = %p\n\n", node->parent, node->data, node->left, node->right);
    ShowTree(fp, node->right, rank, cur_rank);
    fprintf(fp, ")");
    (*cur_rank)--;
    return;
}

Akinator_Errors SaveTreeToFile(binary_tree *tree, FILE *file_to_save)
{
    ASSERTS(tree);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
    {
        return err;
    }

    return err;
}

Akinator_Errors ReadTreeFromFile(binary_tree *tree, char *name_of_file)
{
    ASSERTS(tree);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
    {
        return err;
    }

    FILE *file_to_read = fopen(name_of_file, "r");
    if (!OpenFileSuccess(file_to_read, "tree_for_akinator_game.txt"))
        return ERROR_DURING_OPENING_FILE;

    fseek(file_to_read, SEEK_SET, 0);
    size_t num_of_bytes_in_file = return_num_of_bytes_in_file(fileno(file_to_read));

    char *tree_buffer = (char *)calloc(num_of_bytes_in_file + 1, sizeof(char));
    if (tree_buffer == NULL)
        return ERROR_DURING_MEMORY_ALLOCATION;

    size_t num_success_read_symbols = fread(tree_buffer, sizeof(char), num_of_bytes_in_file, file_to_read);
    if (num_success_read_symbols < num_of_bytes_in_file)
        return ERROR_DURING_READ_FILE;

    printf("%zu\n", num_success_read_symbols); // проверка количества успешно прочитанных в буфер символов
    printf("content = %s\n", tree_buffer);

    tree_buffer[num_success_read_symbols] = '\0';
   
    if (!CloseFileSuccess(file_to_read, "tree_for_akinator_game.txt"))
        return ERROR_DURING_CLOSING_FILE;
    
    char *ptr_to_quotation_mark = tree_buffer;
    size_t a = 0;
    for (size_t i = 0; (ptr_to_quotation_mark = strchr(ptr_to_quotation_mark, '"')) != NULL; i++)
    {
        a++;
        printf("<%s>\n", ptr_to_quotation_mark);
        if (i % 2) *ptr_to_quotation_mark = '\0';
        ptr_to_quotation_mark++;
        //if (ptr_on_first_qm == ptr_to_quotation_mark) break;
        //if (i == 2) ptr_on_first_qm = ptr_to_quotation_mark;
    }

    printf("a = %zu", a);

    static char *position = tree_buffer;
    ReadNodeFromBuffer(tree, &position, &(tree->root), NULL);

    ssize_t rank = 0;
    ssize_t cur_rank = 0;
    ShowTree(stdout, tree->root, &rank, &cur_rank);
    free(tree_buffer);

    printf("tree made\n");
    return err;
}

char *ReadNodeFromBuffer(binary_tree *tree, char **position, node_t **node, node_t *parent)
{
    ASSERTS(tree);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
    {
        return NULL;
    }

    SkipSpaces(position);
    printf("*position_000000 = <%c>\n", **position);

    if (**position == '(')
    {
        (*position)++;
        SkipSpaces(position);
        printf("*position = <%c>\n", **position);

        if (**position == '"')
        {
            (*position)++;
            NodeFromFileInit(tree, position, node, parent);

            *position = strchr(*position, '\0') + 1;
        }

        else return NULL; // TODO: exactly? 
    }

    else if (**position == 'n' && *(*position + 1) == 'i' && *(*position + 2) == 'l' && *(*position + 3) == ' ')
    {
        SkipSpaces(position);

        *position += 3;
        return *position;
    }

    *position = ReadNodeFromBuffer(tree, position, &((*node)->left), *node);
    printf("\n*position_____2 = <%s>\n", *position);
    printf("\n(*node)->data = <%s>\n", (*node)->data);
    *position = ReadNodeFromBuffer(tree, position, &((*node)->right), *node);
    printf("\n*position_____3 = <%s>\n", *position);
    printf("\n(*node)->data = <%s>\n", (*node)->data);

    SkipSpaces(position);
    if (**position == ')')
    {
        (*position)++;
        return *position;
    }

    if ((err = AkinatorVerify(tree)))
    {
        return *position;
    }

    return *position;
}

Akinator_Errors NodeFromFileInit(binary_tree *tree, char **position, node_t **node, node_t *parent)
{
    ASSERTS(tree);
    
    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    if (*node != tree->root)
    {
        *node = (node_t *)calloc(1, sizeof(node_t));
        tree->num_of_el++;
    }
    if (*node == tree->root) free(tree->root->data);
    printf("*******HEAR***********\n");
    (*node)->data = strdup(*position);
    printf("node->data = %s\n", (*node)->data);
    (*node)->parent = parent; 
    (*node)->left = NULL;
    (*node)->right = NULL;

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

size_t return_num_of_bytes_in_file(int fd1)
{
    struct stat st1 = {};
    fstat(fd1, &st1);

    return (size_t)st1.st_size;
}

void SkipSpaces(char **position)
{
    while (**position == ' ') 
    {
        printf("<%c>", **position);
        (*position)++;
        continue;
    };
}

bool OpenFileSuccess(FILE *fp, const char * file_name)
{
    if (fp == NULL)
    {
        printf("An error occurred when opening the file %s", file_name);
        return false;
    }

    return true;
}

bool CloseFileSuccess(FILE *fp, const char * file_name)
{
    if (fclose(fp))
    {
        printf("An error occurred when closing the file %s", file_name);
        return false;
    }
    
    return true;
}

bool PrintError(Akinator_Errors err)
{
    switch (err)
    {
        case NO_ERROR:
            return true;

        case NULL_POINTER_ON_TREE:
            printf("The pointer on the TREE is empty\n");
            return true;

        case TREE_IS_EMPTY:
            printf("The TREE is empty\n");
            return true;

        case ELEMENT_NOT_FOUND:
            printf("The item was not found in the tree\n");
            return true;

        case ERROR_DURING_THE_CREATION_OF_THE_TREE:
            printf("An error occurred when was the tree created\n");
            return true;

        case NEGATIVE_NUM_OF_ELEMENTS:
            printf("The number of elements takes a negative value\n");
            return true;

        case NULL_POINTER_ON_ROOT:
            printf("The pointer on the ROOT is empty\n");
            return true;

        case ROOT_HAVE_PARENT:
            printf("The ROOT have a parent\n");
            return true;

        case SON_HAS_WRONG_PARENT:
            printf("A son has a wrong parent\n");
            return true;

        case ERROR_DURING_OPENING_FILE:
            printf("An error occurred while opening the file\n");
            return true;

        case ERROR_DURING_CLOSING_FILE:
            printf("An error occurred while closing the file\n");
            return true;

        case ERROR_DURING_MEMORY_ALLOCATION:
            printf("Couldn't allocate dynamic memory\n");
            return true;

        case ERROR_DURING_READ_FILE:
            printf("The number of successfully read characters from the file is less than its content\n");
            return true;

        case INCORRECT_DATA_IN_FILE:
            printf("There are incorrect data in the file with tree nodes\n");
            return true;

        default:
            return false;
    }

    return false;
}
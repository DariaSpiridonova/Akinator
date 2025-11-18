#include "akinator.h"

Akinator_Errors AkinatorGame(binary_tree *tree)
{
    ASSERTS(tree);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    ssize_t num = 0;

    do
    {    
        printf("What do you want to do?\n");
        printf("    1) Play a game\n");
        printf("    2) Find an object\n");
        printf("    3) Compare objects\n");
        printf("    4) End the game\n");
        printf("Enter the number of the corresponding request: ");
        GetARequestNumber(&num);

        switch(num)
            {
                case 1:
                    err = PlayGame(tree);
                    break;

                case 2:
                    err =  FindObject(tree);
                    break;

                case 3:
                    err = CompareObjects(tree);
                    break;

                case 4:
                    printf("Thanks for playing! Goodbye!\n");
                    break;

                default:
                    return err;
            }
    }

    while (num != 4);

    return err;
}

void GetARequestNumber(ssize_t *num)
{
    while (scanf("%zd", num) != 1 || *num > 4 || *num < 1) 
    {
        printf("Please, enter a number of the request in the range of 1-3: ");
        DiscardSuperfluous();
    }
   DiscardSuperfluous();
}

Akinator_Errors PlayGame(binary_tree *tree)
{
    ASSERTS(tree);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    node_t *node = tree->root;
    bool answer = false;

    while (true)
    {
        while (node->data[strlen(node->data) - 1]  == '?') 
        {
            AskQuestion(&node);
        }

        printf("Is it %s?\n", node->data);
        answer = GetAnswer();

        if (answer)
        {
            err = Victory(tree);
            if (err) return err;
            break;
        }

        else if (!answer)
        {
            if ((err = AddingNode(tree, node)))
                return err;
            
            break;
        }
    }

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

void AskQuestion(node_t **node)
{
    bool answer = false;
    printf("%s\n", (*node)->data);
    answer = GetAnswer();

    if (answer)
    {
        *node = (*node)->left;
    }
    else 
    {
        *node = (*node)->right;
    }
}

Akinator_Errors AddingNode(binary_tree *tree, node_t *node)
{
    ASSERTS(tree);
    assert(node != NULL);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    char *object = NULL;
    printf("And what is it?\n");
    GetObject(&object);

    char *feature = NULL;
    GetFeature(&feature, object, node);

    NodeInit(tree, &(node->right), node, &(node->data));
    
    NodeInit(tree, &(node->left), node, &object);

    node->data = strdup(feature);
    free(feature);

    printf("Okay, we have included your feature in the game.\n");

    if ((err = AkinatorVerify(tree)))
        return err;
    
    return err;
}

Akinator_Errors Victory(binary_tree *tree)
{
    ASSERTS(tree);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    printf("Hooray! We guessed right!\n");
    return err;
}

Akinator_Errors NodeInit(binary_tree *tree, node_t **node, node_t *parent, char **string)
{
    ASSERTS(tree);
    assert(parent != NULL);
    assert(string != NULL);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    *node = (node_t *)calloc(1, sizeof(node_t));
    tree->num_of_el++;
    (*node)->parent = parent;
    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->data = strdup(*string);
    free(*string);
    *string = NULL;

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

bool GetAnswer()
{
    char *answer = NULL;
    size_t num_of_bytes = 0;
    getline(&answer, &num_of_bytes, stdin);
    answer[strlen(answer) - 1] = '\0';

    while (strcmp(answer, "yes") && strcmp(answer, "no"))
    {
        printf("Please, enter the answer to the question: yes or no\n");
        getline(&answer, &num_of_bytes, stdin);
        answer[strlen(answer) - 1] = '\0';
    }

    if (!strcmp(answer, "yes"))
    {
        free(answer);
        return true;
    }

    free(answer);
    return false;
}

void GetObject(char **ptr)
{
    size_t num_of_bytes = 0;
    getline(ptr, &num_of_bytes, stdin);
    (*ptr)[strlen(*ptr) - 1] = '\0';
}

void GetFeature(char **ptr, char *object, node_t *node)
{
    assert(object != NULL);
    assert(node != NULL);

    bool is_negation = GetDifference(ptr, object, node);
    while (is_negation)
    {
        printf("The feature should not contain negation. Please try again\n");
        is_negation = GetDifference(ptr, object, node);
    }
    (*ptr)[strlen(*ptr) - 1] = '?';
}

bool GetDifference(char **ptr, char *object, node_t *node)
{
    size_t num_of_bytes = 0;
    char first_word[ANSWER_SIZE*2] = {0};

    printf("How is %s different from %s?\n", object, node->data);
    getline(ptr, &num_of_bytes, stdin);
    sscanf(*ptr, "%8s", first_word);
    first_word[ANSWER_SIZE*2 - 1] = '\0';
    for (size_t i = 0; first_word[i] != '\0'; i++)
    {
        first_word[i] = (char)tolower(first_word[i]);
    }

    if (!strcmp(first_word, "not") || !strcmp(first_word, "no") || !strcmp(first_word, "never"))
        return true;
    
    return false;
}

Akinator_Errors FindObject(binary_tree *tree)
{
    ASSERTS(tree);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    char *object = NULL;
    printf("Which object do you want to find?\n");
    GetObject(&object);

    bool object_was_found = false;
    node_t *ptr_object = NULL;

    node_t **ptrs_on_nodes = (node_t **)calloc((size_t)tree->num_of_el, sizeof(node_t *));
    size_t index = 0;

    if (!FindObjectRecursive(tree, tree->root, object, &ptr_object, &object_was_found, ptrs_on_nodes, &index))
    {
        printf("The item was not found in the tree\n");
    }

    else    
    {
        GetDescription(tree, ptr_object, ptrs_on_nodes, 0);
    }

    free(object);
    free(ptrs_on_nodes);

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

bool FindObjectRecursive(binary_tree *tree, node_t *node, char *object, node_t **ptr_object, bool *object_was_found, node_t **ptrs_on_nodes, size_t *index)
{
    ASSERTS(tree);
    assert(object != NULL);

    if (*object_was_found) return true;

    if (node == NULL) return *object_was_found;

    ptrs_on_nodes[(*index)++] = node;

    FindObjectRecursive(tree, node->left,  object, ptr_object, object_was_found, ptrs_on_nodes, index);
    FindObjectRecursive(tree, node->right, object, ptr_object, object_was_found, ptrs_on_nodes, index);

    if (!strcmp(object, node->data)) 
    {
        *ptr_object = node;
        *object_was_found = true;
    }

    else if (strcmp(object, node->data) && !*object_was_found)
    {
        ptrs_on_nodes[--(*index)] = NULL;
    }

    return *object_was_found;
}

void GetDescription(binary_tree *tree, node_t * ptr_object, node_t **ptrs_on_nodes, size_t i)
{
    ASSERTS(tree);

    if (ptr_object == tree->root) printf("It is the root element and has no properties.");
    else
    {
        printf("The signs inherent in %s: \n", ptr_object->data);
        //node_t *node = ptr_object;

        // method 1
        /*for (size_t i = 1; node != tree->root; i++, node = node->parent)
        {
            if (!strcmp(node->parent->right->data, node->data))
                printf("%zu. no %.*s\n", i, (int)strlen(node->parent->data) - 1, node->parent->data);
            else 
                printf("%zu. %.*s\n", i, (int)strlen(node->parent->data) - 1, node->parent->data);
        }

        for (size_t i = 0; ptrs_on_nodes[i] != NULL && ptrs_on_nodes[i + 1] != NULL; i++)
        {
            printf("%s\n", ptrs_on_nodes[i]->data);
        }*/

        // method 2
        for (; ptrs_on_nodes[i] != NULL && ptrs_on_nodes[i + 1] != NULL; i++)
        {
            if (!strcmp(ptrs_on_nodes[i]->right->data, ptrs_on_nodes[i + 1]->data))
                printf("%zu. no %.*s\n", i + 1, (int)strlen(ptrs_on_nodes[i]->data) - 1, ptrs_on_nodes[i]->data);
            else 
                printf("%zu. %.*s\n", i + 1, (int)strlen(ptrs_on_nodes[i]->data) - 1, ptrs_on_nodes[i]->data);
        }
    }
}

Akinator_Errors CompareObjects(binary_tree *tree)
{
    ASSERTS(tree);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    printf("Which objects do you want to compare?\n");
    
    printf("Enter the name of the first object: ");
    char *object1 = NULL;
    GetObject(&object1);

    bool object1_was_found = false;
    node_t *ptr_object1 = NULL;
    node_t **ptrs1_on_nodes = (node_t **)calloc((size_t)tree->num_of_el, sizeof(node_t *));
    size_t index1 = 0;
    
    printf("Enter the name of the second object: ");
    char *object2 = NULL;
    GetObject(&object2);

    bool object2_was_found = false;
    node_t *ptr_object2 = NULL;
    node_t **ptrs2_on_nodes = (node_t **)calloc((size_t)tree->num_of_el, sizeof(node_t *));
    size_t index2 = 0;

    bool not_root_element = true;

    if (!FindObjectRecursive(tree, tree->root, object1, &ptr_object1, &object1_was_found, ptrs1_on_nodes, &index1))
    {
        printf("The item number 1 was not found for comparison in the tree\n");
    }

    if (!FindObjectRecursive(tree, tree->root, object2, &ptr_object2, &object2_was_found, ptrs2_on_nodes, &index2))
    {
        printf("The item number 2 was not found for comparison in the tree\n");
    }

    if (object1_was_found && object2_was_found)
    {
        compare_found_items(tree, &not_root_element, ptrs1_on_nodes, ptrs2_on_nodes, ptr_object1, ptr_object2);
    }

    free_objects(object1, object2, ptrs1_on_nodes, ptrs2_on_nodes);

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

void compare_found_items(binary_tree *tree, bool *not_root_element, node_t **ptrs1_on_nodes, node_t **ptrs2_on_nodes, node_t *ptr_object1, node_t *ptr_object2)
{
    if (!strcmp(ptr_object1->data, ptr_object2->data) && !strcmp(ptr_object1->data, tree->root->data))
    {
        printf("%s is the root element and has no properties.", ptr_object1->data);
        *not_root_element = false;
    }

    if (not_root_element && strcmp(ptr_object1->data, ptr_object2->data)) 
        printf("General signs of objects %s and %s:\n", ptr_object1->data, ptr_object2->data);

    if (not_root_element && !strcmp(ptr_object1->data, ptr_object2->data))  
        printf("Signs of object %s:\n", ptr_object1->data);

    size_t i1 = 0, i2 = 0;

    if (strcmp(ptrs1_on_nodes[i1 + 1]->data, ptrs2_on_nodes[i2 + 1]->data))
    {
        printf("There are no general signs\n");
    }

    print_general_signs(not_root_element, ptrs1_on_nodes, ptrs2_on_nodes, &i1, &i2);

    print_differences(tree, ptrs1_on_nodes, ptrs2_on_nodes, ptr_object1, ptr_object2, &i1, &i2);
}

void print_general_signs(bool not_root_element, node_t **ptrs1_on_nodes, node_t **ptrs2_on_nodes, size_t *i1, size_t *i2)
{
    while (not_root_element && ptrs1_on_nodes[*i1 + 1] != NULL && ptrs2_on_nodes[*i2 + 1] != NULL && !strcmp(ptrs1_on_nodes[*i1 + 1]->data, ptrs2_on_nodes[*i2 + 1]->data))
    {
        if (!strcmp(ptrs1_on_nodes[*i1]->right->data, ptrs1_on_nodes[*i1 + 1]->data))
            printf("%zu. no %.*s\n", *i1 + 1, (int)strlen(ptrs1_on_nodes[*i1]->data) - 1, ptrs1_on_nodes[*i1]->data);
        else 
            printf("%zu. %.*s\n", *i1 + 1, (int)strlen(ptrs1_on_nodes[*i1]->data) - 1, ptrs1_on_nodes[*i1]->data);
        (*i1)++;
        (*i2)++;
    }
}

void print_differences(binary_tree *tree, node_t **ptrs1_on_nodes, node_t **ptrs2_on_nodes, node_t *ptr_object1, node_t *ptr_object2, size_t *i1, size_t *i2)
{
    if (ptrs1_on_nodes[*i1 + 1] != NULL && ptrs2_on_nodes[*i2 + 1] != NULL)
    {
        printf("%s and %s have some differences\n", ptr_object1->data, ptr_object2->data);

        GetDescription(tree, ptr_object1, ptrs1_on_nodes, *i1);
        GetDescription(tree, ptr_object2, ptrs2_on_nodes, *i2);
    }
}

void free_objects(char *object1, char *object2, node_t **ptrs1_on_nodes, node_t **ptrs2_on_nodes)
{
    free(object1);
    free(ptrs1_on_nodes);

    free(object2);
    free(ptrs2_on_nodes);
}

void DiscardSuperfluous()
{
    int ch = 0;
    while ((ch = getchar()) != '\n') continue;
}
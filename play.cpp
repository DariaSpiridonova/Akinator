#include "akinator.h"

Akinator_Errors AkinatorGame(binary_tree *tree)
{
    ASSERTS(tree);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    printf("tree->root = %s\n", tree->root->data);

    printf("What do you want to do?\n");
    printf("    1) Play a game\n");
    printf("    2) Find an object\n");
    printf("    3) Compare objects\n");
    printf("    4) End the game\n");
    printf("Enter the number of the corresponding request: ");
    ssize_t num = 0;
    GetARequestNumber(&num);

    switch(num)
    {
        case 1:
            return PlayGame(tree);

        case 2:
            return FindObject(tree);

        case 3:
            return CompareObjects(tree);

        case 4:
            printf("Thanks for playing! Goodbye!\n");
            return err;

        default:
            return err;
    }

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
            
            if ((err = AkinatorGame(tree)))
            {
                return err;
            }
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
    if (node->data == NULL)
    printf("node->data = \n");

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
    return AkinatorGame(tree);
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
        printf("<%s>", answer);
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
    printf("%zd\n", getline(ptr, &num_of_bytes, stdin));
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

    printf("first_word = %s\n", first_word);

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
    if (!FindObjectRecursive(tree, tree->root, object, &ptr_object, &object_was_found))
    {
        printf("The item was not found in the tree\n");
    }
    else    
    {
        printf("%s: \n", ptr_object->data);
        GetDescription(tree, ptr_object);
    }

    free(object);

    if ((err = AkinatorVerify(tree)))
        return err;

    if ((err = AkinatorGame(tree)))
    {
        return err;
    }

    return err;
}

bool FindObjectRecursive(binary_tree *tree, node_t *node, char *object, node_t **ptr_object, bool *object_was_found)
{
    ASSERTS(tree);
    assert(object != NULL);

    if (*object_was_found) return true;

    if (node == NULL) return *object_was_found;

    FindObjectRecursive(tree, node->left, object, ptr_object, object_was_found);
    FindObjectRecursive(tree, node->right, object, ptr_object, object_was_found);

    if (!strcmp(object, node->data)) 
    {
        *ptr_object = node;
        *object_was_found = true;
    }

    return *object_was_found;
}

void GetDescription(binary_tree *tree, node_t * ptr_object)
{
    ASSERTS(tree);

    printf("%s: \n", ptr_object->data);
    if (ptr_object == tree->root) printf("It is the root element and has no properties.");
    else
    {
        printf("The signs inherent in %s: \n", ptr_object->data);
        node_t *node = ptr_object;
        for (size_t i = 1; node != tree->root; i++, node = node->parent)
        {
            if (!strcmp(node->parent->right->data, node->data))
                printf("%zu. no %.*s\n", i, (int)strlen(node->parent->data) - 1, node->parent->data);
            else 
                printf("%zu. %.*s\n", i, (int)strlen(node->parent->data) - 1, node->parent->data);
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
    
    printf("Enter the name of the second object: ");
    char *object2 = NULL;
    GetObject(&object2);

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

void DiscardSuperfluous()
{
    int ch = 0;
    while ((ch = getchar()) != '\n') continue;
}
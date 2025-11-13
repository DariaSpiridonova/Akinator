#include "akinator.h"

Akinator_Errors AkinatorGame(binary_tree *tree)
{
    assert(tree != NULL);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    printf("What do you want to do?\n");
    printf("    1) Play a game\n");
    printf("    2) Find an object\n");
    printf("    3) Compare objects\n");
    printf("    4) End the game\n");
    printf("Enter the number of the corresponding request: ");
    ssize_t num = 0;
    get_a_request_number(&num);

    switch(num)
    {
        case 1:
            return play_game(tree);

        case 2:
            return find_object(tree);

        case 3:
            return compare_objects(tree);

        case 4:
            printf("Thanks for playing! Goodbye!\n");
            return err;

        default:
            return err;
    }

    return err;
}

void get_a_request_number(ssize_t *num)
{
    while (scanf("%zd", num) != 1 || *num > 4 || *num < 1) 
    {
        printf("Please, enter a number of the request in the range of 1-3: ");
        discard_superfluous();
    }
    discard_superfluous();
}

Akinator_Errors play_game(binary_tree *tree)
{
    assert(tree != NULL);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    node_t *node = tree->root;
    char *answer = NULL;

    while (true)
    {
        // traverse
        while (node->data[strlen(node->data) - 1]  == '?') // is question
        {
            get_answer(&answer); // TODO: get_answer может сразу возвращать bool + проверка ввода
            if (!strcmp(answer, "yes"))
            {
                free(answer);
                node = node->left;
            }

            else if (!strcmp(answer, "no"))
            {
                free(answer);
                node = node->right;
            }
        }

        printf("Is it %s?\n", node->data);
        get_answer(&answer);

        // TODO: Victory
        if (!strcmp(answer, "yes"))
        {
            free(answer);
            printf("Hooray! We guessed right!\n");
            if ((err = AkinatorGame(tree)))
            {
                return err;
            }
            break;
        }
        // TODO: adding
        else if (!strcmp(answer, "no"))
        {
            free(answer);
            char *object = NULL;
            printf("And what is it?\n");
            get_object(&object);

            char *feature = NULL;
            get_feature(&feature, object, node);

            NodeInit(tree, &(node->right), node, &(node->data));
            
            NodeInit(tree, &(node->left), node, &object);
            if (node->data == NULL)
            printf("node->data = \n");

            //node->data = feature;
            node->data = (char *)calloc(1, sizeof(char) * (strlen(feature) + 1));
            strcpy(node->data, feature);
            free(feature);

            printf("Okay, we have included your feature in the game.\n");
            
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

Akinator_Errors NodeInit(binary_tree *tree, node_t **node, node_t *parent, char **string)
{
    assert(tree != NULL);
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
    (*node)->data = (char *)calloc(1, sizeof(char) * (strlen(*string) + 1));
    strcpy((*node)->data, *string); // FIXME: strdup
    free(*string);
    *string = NULL;

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

bool start_again(char **answer)
{
    printf("Do you want to start playing again?\n");
    get_answer(answer);
    if (!strcmp(*answer, "yes")) 
    {
        return true; 
    }

    else return false;
}

void get_answer(char **answer)
{
    size_t num_of_bytes = 0;
    getline(answer, &num_of_bytes, stdin);
    (*answer)[strlen(*answer) - 1] = '\0';
    //scanf("%4s", answer);
    //answer[ANSWER_SIZE] = '\0';
    while (strcmp(*answer, "yes") && strcmp(*answer, "no"))
    {
        printf("<%s>", *answer);
        printf("Please, enter the answer to the question: yes or no\n");
        getline(answer, &num_of_bytes, stdin);
        (*answer)[strlen(*answer) - 1] = '\0';
    }
}

void get_object(char **ptr)
{
    size_t num_of_bytes = 0;
    printf("%zd\n", getline(ptr, &num_of_bytes, stdin));
    (*ptr)[strlen(*ptr) - 1] = '\0';
}

void get_feature(char **ptr, char *object, node_t *node)
{
    assert(object != NULL);
    assert(node != NULL);

    // TODO: remove copypaste
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
    while (!strcmp(first_word, "not") || !strcmp(first_word, "no") || !strcmp(first_word, "never"))
    {
        printf("The feature should not contain negation. Please try again\n");
        printf("How is %s different from %s?\n", object, node->data);
        getline(ptr, &num_of_bytes, stdin);
        sscanf(*ptr, "%8s", first_word);
        first_word[ANSWER_SIZE*2 - 1] = '\0';
        for (size_t i = 0; first_word[i] != '\0'; i++)
        {
            first_word[i] = (char)tolower(first_word[i]);
        }
    }
    (*ptr)[strlen(*ptr) - 1] = '?';
    printf("first_word = %s\n", first_word);
}

Akinator_Errors find_object(binary_tree *tree)
{
    assert(tree != NULL);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    char *object = NULL;
    printf("Which object do you want to find?\n");
    get_object(&object);

    bool flag = false;
    node_t *ptr_object = NULL;
    if (!find_object_recursive(tree, tree->root, object, &ptr_object, &flag))
    {
        free(object);
        return ELEMENT_NOT_FOUND;
    }
    else    
    {
        printf("%s: \n", ptr_object->data);
        get_description(tree, ptr_object);
    }

    free(object);

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

// FIXME: flag плохое название
bool find_object_recursive(binary_tree *tree, node_t *node, char *object, node_t **ptr_object, bool *flag)
{
    assert(object != NULL);

    if (*flag) return true;

    if (node == NULL) return *flag;

    find_object_recursive(tree, node->left, object, ptr_object, flag);
    find_object_recursive(tree, node->right, object, ptr_object, flag);

    if (!strcmp(object, node->data)) 
    {
        *ptr_object = node;
        *flag = true;
    }

    return *flag;
}

void get_description(binary_tree *tree, node_t * ptr_object)
{
    printf("%s: \n", ptr_object->data);
    if (ptr_object == tree->root) printf("It is the root element and has no properties.");
    else
    {
        printf("The signs inherent in %s: \n", ptr_object->data);
        node_t *node = ptr_object;
        for (size_t i = 1; node != tree->root; i++)
        {
            printf("%zu. %s\n", i, node->parent->data);
        }
    }
}

Akinator_Errors compare_objects(binary_tree *tree)
{
    assert(tree != NULL);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    printf("Which objects do you want to compare?\n");
    
    printf("Enter the name of the first object: ");
    char *object1 = NULL;
    get_object(&object1);
    
    printf("Enter the name of the second object: ");
    char *object2 = NULL;
    get_object(&object2);

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

void discard_superfluous()
{
    int ch = 0;
    while ((ch = getchar()) != '\n') continue;
}
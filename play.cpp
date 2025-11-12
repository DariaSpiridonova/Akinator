#include "akinator.h"

Akinator_Errors akinator_game(binary_tree *tree)
{
    assert(tree != NULL);

    printf("What do you want to do?\n");
    printf("    1) Play a game\n");
    printf("    2) Find an object\n");
    printf("    3) Compare objects\n");
    printf("Enter the number of the corresponding request: ");
    ssize_t num = 0;
    get_a_request_number(&num);

    if (num == 1) play_game(tree);
    if (num == 2) find_an_object(tree);
    if (num == 3) compare_objects(tree);

    return NO_ERROR;
}

void get_a_request_number(ssize_t *num)
{
    while (scanf("%zd", num) != 1 || *num > 3 || *num < 1) 
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
        while (node->data[strlen(node->data) - 1]  == '?')
        {
            printf("%s\n", node->data);
            get_answer(&answer);
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

        if (!strcmp(answer, "yes"))
        {
            free(answer);
            printf("Hooray! We guessed right!\n");
            if (start_again(&answer)) 
            {
                free(answer);
                node = tree->root;
                continue; 
            }

            else 
            {
                free(answer);
                break;
            }
        }

        else if (!strcmp(answer, "no"))
        {
            free(answer);
            char *object = NULL;
            printf("And what is it?\n");
            get_object(&object);

            char *feature = NULL;
            get_feature(&feature, object, node);

            NodeInit(tree, &node->right, node, node->data);
            
            NodeInit(tree, &node->left, node, object);

            //node->data = feature;
            node->data = (char *)calloc(1, sizeof(char) * (strlen(feature) + 1));
            strcpy(node->data, feature);
            free(feature);

            printf("Okay, we have included your feature in the game.\n");
            if (start_again(&answer)) 
            {
                free(answer);
                node = tree->root;
                continue; 
            }

            else 
            {
                free(answer);
                break;
            }
        }
    }

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
}

Akinator_Errors NodeInit(binary_tree *tree, node_t **node, node_t *parent, char *string)
{
    assert(tree != NULL);
    assert(parent != NULL);
    assert(string != NULL);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    *node = (node_t *)calloc(1, sizeof(node_t));
    (*node)->parent = parent;
    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->data = (char *)calloc(1, sizeof(char) * (strlen(string) + 1));
    strcpy((*node)->data, string);
    free(string);

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

Akinator_Errors find_an_object(binary_tree *tree)
{
    assert(tree != NULL);

    Akinator_Errors err = NO_ERROR;
    if ((err = AkinatorVerify(tree)))
        return err;

    char *object = NULL;
    printf("Which object do you want to find?\n");
    get_object(&object);

    if ((err = AkinatorVerify(tree)))
        return err;

    return err;
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
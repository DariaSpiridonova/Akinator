#include "akinator.h"

int main()
{
    binary_tree akinator_tree;

    Akinator_Errors err = NO_ERROR;

    if ((err = AkinatorInit(&akinator_tree, "logfile_for_tree1.htm")))
    {
        PrintError(err);
        return 2;
    }

    if ((err = ReadTreeFromFile(&akinator_tree, "tree_for_akinator_game.txt")))
    {
        PrintError(err);
        return 3;
    }
        
    if ((err = AkinatorGame(&akinator_tree)))
    {
        PrintError(err);
        return 4;
    }
        
    AKINATOR_DUMP(&akinator_tree);

    if ((err = AkinatorDestroy(&akinator_tree)))
    {
        PrintError(err);
        return 5;
    }
        
    printf("akinator_tree.root_adress = %p\n", akinator_tree.root);
    printf("thats all\n");

    return 0;
}
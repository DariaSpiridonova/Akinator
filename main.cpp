#include "akinator.h"

int main()
{
    binary_tree akinator_tree;

    AkinatorInit(&akinator_tree, "logfile_for_tree1.htm");
    akinator_game(&akinator_tree);
    AKINATOR_DUMP(&akinator_tree);
    AkinatorDestroy(&akinator_tree, akinator_tree.root);
    AKINATOR_DUMP(&akinator_tree);
    printf("thats all\n");

    return 0;
}
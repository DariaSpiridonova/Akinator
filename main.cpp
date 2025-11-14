#include "akinator.h"

int main()
{
    // Хочу две функции!
    // TREE -> file (сохранение дерево в файл)
    // file -> TREE (чтение дерева из файла)
    binary_tree akinator_tree;

    Akinator_Errors err = NO_ERROR;

    if ((err = AkinatorInit(&akinator_tree, "logfile_for_tree1.htm")))
        PrintError(err);
    if ((err = AkinatorGame(&akinator_tree))) // FIXME: выбрать кодстайл и привести все функции
        PrintError(err);
    AKINATOR_DUMP(&akinator_tree);
    if ((err = AkinatorDestroy(&akinator_tree)))
        PrintError(err);
    //printf("akinator_tree.root.left_adress = %p\n", akinator_tree.root->left);
    printf("akinator_tree.root_adress = %p\n", akinator_tree.root);
    printf("thats all\n");

    return 0;
}
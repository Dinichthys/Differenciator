#include "differenciator.h"

#include <stdio.h>
#include <stdlib.h>

#include "../My_lib/Logger/logging.h"

#define ERROR_HANDLER(error)                                                                            \
    if (error != kDoneDiff)                                                                             \
    {                                                                                                   \
        fprintf (stderr, "Code error = {%d} with name \"%s\"\n",                                        \
                         error, EnumErrorToStr (error));                                                \
        FuncDtor (root);                                                                                \
        fclose (error_file);                                                                            \
        return EXIT_FAILURE;                                                                            \
    }

int main ()
{
    FILE* const error_file = fopen ("My_lib/Logger/error.txt", "w");
    if (error_file == NULL)
    {
        fprintf (stderr, "Can't start logging\n");
        return EXIT_FAILURE;
    }
    // set_log_file (error_file);
    set_log_lvl (DEBUG);

    // node_t* root = NULL;

    // node_t* root = FuncCtor ();

    node_t* root = FuncCtor ();

    enum DiffError result = kDoneDiff;

//
//     node_t set_val = {};
//     set_val.type = kFunc;
//     set_val.value.function = kSin;
//     set_val.parent = set_val.left = set_val.right = NULL;
//
//     root = AddNode_ (set_val);
//     // root->left = AddNode_ ({kNum, 2, NULL, NULL, NULL});
//     root->right = AddNode_ ({kNum, 1.5, NULL, NULL, NULL});
//
    result = ReadDataBase ("Example.txt", &root);

    ERROR_HANDLER (result);

    result = DumpDiff (root);

    ERROR_HANDLER (result);

    root = Simplify (root);

    result = DumpDiff (root);

    ERROR_HANDLER (result);

    fprintf (stderr, "%lf\n", root->value.number);

    FuncDtor (root);

    fclose (error_file);

    return EXIT_SUCCESS;
}

#undef ERROR_HANDLER

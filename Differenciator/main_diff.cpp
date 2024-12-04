#include "differenciator.h"

#include <stdlib.h>
#include <stdio.h>

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
    set_log_file (error_file);
    set_log_lvl (DEBUG);

    node_t* root = FuncCtor ();

    enum DiffError result = kDoneDiff;

    result = ReadDataBase ("Example.txt", &root);

    ERROR_HANDLER (result);

    result = DumpDiff (root);

    ERROR_HANDLER (result);

    root = Simplify (root);

    result = DumpDiff (root);

    ERROR_HANDLER (result);

    node_t* new_root = NULL;

    result = Differencing (&new_root, root);

    root = new_root;

    result = DumpDiff (root);

    ERROR_HANDLER (result);

    root = Simplify (root);

    result = DumpDiff (root);

    ERROR_HANDLER (result);

    // fprintf (stderr, "%lf\n", root->value.number);

    FuncDtor (root);

    fclose (error_file);

    return EXIT_SUCCESS;
}

#undef ERROR_HANDLER

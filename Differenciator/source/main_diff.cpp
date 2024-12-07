#include "differenciator.h"

#include <stdlib.h>
#include <stdio.h>

#include "My_lib/Logger/logging.h"

#define ERROR_HANDLER(error)                                                                            \
    if (error != kDoneDiff)                                                                             \
    {                                                                                                   \
        fprintf (stderr, "Code error = {%d} with name \"%s\"\n",                                        \
                         error, EnumErrorToStr (error));                                                \
        ExpressionDtor (root);                                                                          \
        PrintEndTex (dump_tex_file, "/Dump_Files", "Dump_Files/Dump_Tex.tex");                          \
        fclose (dump_tex_file);                                                                         \
        fclose (error_file);                                                                            \
        return EXIT_FAILURE;                                                                            \
    }

int main () // FIXME argc argv принимать файлы
{
    FILE* const error_file = fopen ("Differenciator/libs/My_lib/Logger/error.txt", "w");
    if (error_file == NULL)
    {
        fprintf (stderr, "Can't start logging\n");
        return EXIT_FAILURE;
    }
    set_log_file (error_file);
    set_log_lvl (kDebug);

    FILE* const dump_tex_file = fopen ("Dump_Files/Dump_Tex.tex", "w");
    if (dump_tex_file == NULL)
    {
        fprintf (stderr, "Cant't start tex dump\n");
        fclose  (error_file);
        return EXIT_FAILURE;
    }
    PrintTitleTex (dump_tex_file);

    node_t* root = ExpressionCtor ();

    enum DiffError result = kDoneDiff;

    result = ReadDataBase ("Example.txt", &root, dump_tex_file);
    ERROR_HANDLER (result);

    result = DumpDiff (root);
    ERROR_HANDLER (result);

    root = Simplify (root, dump_tex_file);

    PrintSimplificationEnd (root, dump_tex_file);

    result = DumpDiff (root);
    ERROR_HANDLER (result);

    node_t* new_root = NULL;

    result = Differencing (&new_root, root, dump_tex_file);

    root = new_root;

    result = DumpDiff (root); // REVIEW -  лучше не делать пустых строк между получением ошибки и обработкой
    ERROR_HANDLER (result);

    root = Simplify (root, dump_tex_file);

    PrintSimplificationEnd (root, dump_tex_file);

    result = DumpDiff (root);
    ERROR_HANDLER (result);

    ExpressionDtor (root);

    PrintEndTex (dump_tex_file, "/Dump_Files", "Dump_Files/Dump_Tex.tex");

    fclose (dump_tex_file);
    fclose (error_file);

    return EXIT_SUCCESS;
}

#undef ERROR_HANDLER

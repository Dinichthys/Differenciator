#include "differenciator.h"
#include "parse_flags_diff.h"
#include "parse_mode_diff.h"

#include <stdlib.h>
#include <stdio.h>

#include "My_lib/Logger/logging.h"

#define ERROR_HANDLER(error)                                                                            \
    if (error != kDoneDiff)                                                                             \
    {                                                                                                   \
        fprintf (stderr, "Code error = {%d} with name \"%s\"\n",                                        \
                         error, EnumErrorToStr (error));                                                \
        ExpressionDtor (root);                                                                          \
        PrintEndTex (set.stream_out, set.output_file_name);                                             \
        SettingsDtor (&set);                                                                            \
        return EXIT_FAILURE;                                                                            \
    }

int main (const int argc, char* argv[]) // REVIEW argc argv принимать файлы
{
    settings_of_program_t set = {};
    SettingsCtor (&set);
    ParseFlags (argc, argv, &set);
    if (set.stop_program)
    {
        SettingsDtor (&set);
        return EXIT_FAILURE;
    }

    set_log_file (set.stream_err);
    set_log_lvl (kDebug);

    PrintTitleTex (set.stream_out);

    node_t* root = ExpressionCtor ();

    enum DiffError result = kDoneDiff;

    result = ReadExpression (&set, &root);
    ERROR_HANDLER (result);

    result = DumpDiff (root);
    ERROR_HANDLER (result);

    root = Simplify (root, set.stream_out);

    PrintSimplificationEnd (root, set.stream_out);

    result = DumpDiff (root);
    ERROR_HANDLER (result);

    result = ParseMode (&set, &root);
    ERROR_HANDLER (result);

    result = DumpDiff (root); // REVIEW -  лучше не делать пустых строк между получением ошибки и обработкой
    ERROR_HANDLER (result);

    root = Simplify (root, set.stream_out);

    PrintSimplificationEnd (root, set.stream_out);

    result = DumpDiff (root);
    ERROR_HANDLER (result);

    ExpressionDtor (root);

    PrintEndTex (set.stream_out, set.output_file_name);

    SettingsDtor (&set);

    return EXIT_SUCCESS;
}

#undef ERROR_HANDLER

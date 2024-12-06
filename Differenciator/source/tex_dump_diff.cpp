#include "../differenciator.h"

#include <stdio.h>
#include <time.h>

#include "../../My_lib/Assert/my_assert.h"
#include "../../My_lib/Logger/logging.h"
#include "../../My_lib/My_stdio/my_stdio.h"

static enum DiffError PrintTitleTex       (FILE* const dump_file);
static enum DiffError PrintTreeDiffTex    (const node_t* const root, FILE* const dump_file);
static enum DiffError PrintFuncDiff       (const node_t* const root, FILE* const dump_file);
static enum DiffError PrintNumVarNodeDiff (const node_t* const root, FILE* const dump_file);

enum DiffError DumpTexTreeDiff (const node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",      root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);

    enum DiffError result = kDoneDiff;

    if (ftell (dump_file) == 0)
    {
        result = PrintTitleTex (dump_file);
    }

    fprintf (dump_file, "\\[\n");
    result = PrintTreeDiffTex (root, dump_file);
    fprintf (dump_file, "\n\\]\n"
                        "\\newline");

    return result;
}

enum DiffError PrintEndTex (FILE* const dump_file, const char* const directory_tex, const char* const file_name)
{
    ASSERT (dump_file     != NULL, "Invalid argument dump_file = %p\n", dump_file);
    ASSERT (directory_tex != NULL, "Invalid argument Directory = %p\n", directory_tex);
    ASSERT (file_name     != NULL, "Invalid argument File name = %p\n", file_name);

    fprintf (dump_file, "\n\\end{document}\n");

    char command [kCommandTerminalDumpLen] = "";

    sprintf (command, "pdflatex %s", file_name);
    if (system (command) == -1)
    {
        return kCantMakePdfTexDiff;
    }

    return kDoneDiff;
}

static enum DiffError PrintTitleTex (FILE* const dump_file)
{
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);

    setbuf (dump_file, NULL);

    char data [kLenDataTex] = "";

    time_t now = time (NULL);

    strftime (data, kLenDataTex, "%B %Y", localtime (&now));

    fprintf (dump_file, "\\documentclass{article}\n"
                        "\n\\usepackage[T2A]{fontenc}\n"
                        "\\usepackage[english, russian]{babel}\n"
                        "\n\\title{Чудо отечественного производства}\n"
                        "\\author{Dinichthys}\n"
                        "\\date{%s}\n\n"
                        "\\begin{document}\n"
                        "\t\\maketitle\n",
                        data);

    return kDoneDiff;
}

static enum DiffError PrintTreeDiffTex (const node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",      root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);

    enum DiffError result = kDoneDiff;

    if (root->type != kFunc)
    {
        result = PrintNumVarNodeDiff (root, dump_file);
        if (result != kDoneDiff)
        {
            return result;
        }
    }

    result = PrintFuncDiff (root, dump_file);

    return kDoneDiff;
}

static enum DiffError PrintFuncDiff (const node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",      root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);

    enum DiffError result = kUndefFuncPrintTexDiff;

    if (root->type != kFunc)
    {
        return kInvalidNodeForPrintTexDiff;
    }

    #define BINARY_FUNC_INFIX(func_type, str_func)                      \
        if (root->value.function == func_type)                          \
        {                                                               \
            fprintf (dump_file, " {");                                   \
            result = PrintTreeDiffTex (root->left,  dump_file);         \
            fprintf (dump_file, "} ");                                   \
            if (result != kDoneDiff)                                    \
            {                                                           \
                return result;                                          \
            }                                                           \
            fprintf (dump_file, str_func);                              \
            fprintf (dump_file, " {");                                   \
            result = PrintTreeDiffTex (root->right, dump_file);         \
            fprintf (dump_file, "} ");                                   \
            return result;                                              \
        }

    BINARY_FUNC_INFIX (kAdd, "+");
    BINARY_FUNC_INFIX (kSub, "-");
    BINARY_FUNC_INFIX (kMul, "*");
    BINARY_FUNC_INFIX (kPow, "^");

    #undef BINARY_FUNC_INFIX

    #define BINARY_FUNC_PREFIX(func_type, str_func)                     \
        if (root->value.function == func_type)                          \
        {                                                               \
            fprintf (dump_file, str_func);                              \
            fprintf (dump_file, " {");                                   \
            result = PrintTreeDiffTex (root->left,  dump_file);         \
            fprintf (dump_file, "} ");                                   \
            if (result != kDoneDiff)                                    \
            {                                                           \
                return result;                                          \
            }                                                           \
            fprintf (dump_file, " {");                                   \
            result = PrintTreeDiffTex (root->right, dump_file);         \
            fprintf (dump_file, "} ");                                   \
            return result;                                              \
        }

    BINARY_FUNC_PREFIX (kDiv, "\\frac");
    BINARY_FUNC_PREFIX (kLog, "\\log_");

    #undef BINARY_FUNC_PREFIX

    #define UNARY_FUNC(func_type, str_func)                             \
        if (root->value.function == func_type)                          \
        {                                                               \
            fprintf (dump_file, str_func);                              \
            fprintf (dump_file, " {");                                   \
            result = PrintTreeDiffTex (root->right, dump_file);         \
            fprintf (dump_file, "} ");                                   \
            return result;                                              \
        }

    UNARY_FUNC (kSin, "\\sin");
    UNARY_FUNC (kCos, "\\cos");
    UNARY_FUNC (kTg , "\\tan");
    UNARY_FUNC (kCtg, "\\cot");

    UNARY_FUNC (kLn,  "\\ln");

    #undef UNARY_FUNC

    return result;
}

static enum DiffError PrintNumVarNodeDiff (const node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",      root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);

    if (root->type == kNum)
    {
        fprintf (dump_file, " %.3lf ", root->value.number);

        return kDoneDiff;
    }

    if (root->type == kVar)
    {
        fprintf (dump_file, " %c ", root->value.variable);

        return kDoneDiff;
    }

    return kInvalidNodeForPrintTexDiff;
}

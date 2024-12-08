#include "tex_dump_diff.h"

#include <stdio.h>
#include <time.h>

#include "My_lib/Assert/my_assert.h"
#include "My_lib/Logger/logging.h"
#include "My_lib/My_stdio/my_stdio.h"

static enum DiffError PrintTreeDiffTex      (const node_t* const root, FILE* const dump_file);
static enum DiffError PrintFuncDiff         (const node_t* const root, FILE* const dump_file);
static enum DiffError PrintNumVarNodeDiff   (const node_t* const root, FILE* const dump_file);
static enum DiffError PrintBinaryInfixFunc  (const node_t* const root, const enum FuncType func_type,
                                             const char* const str_func, FILE* const dump_file);
static enum DiffError PrintBinaryPrefixFunc (const node_t* const root, const enum FuncType func_type,
                                             const char* const str_func, FILE* const dump_file);
static enum DiffError PrintUnaryFunc        (const node_t* const root, const enum FuncType func_type,
                                             const char* const str_func, FILE* const dump_file);

enum DiffError PrintFormula (const node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",      root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);

    enum DiffError result = kDoneDiff;

    fprintf (dump_file, "\\[\n");
    result = PrintTreeDiffTex (root, dump_file);
    fprintf (dump_file, "\n\\]\n"
                        "\\newline\n");

    return result;
}

enum DiffError PrintTitleTex (FILE* const dump_file)
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
                        "\\maketitle\n"
                        "\\newpage\n",
                        data);

    return kDoneDiff;
}

enum DiffError PrintEndTex (FILE* const dump_file, const char* const file_name)
{
    ASSERT (dump_file     != NULL, "Invalid argument dump_file = %p\n", dump_file);
    ASSERT (file_name     != NULL, "Invalid argument File name = %p\n", file_name);

    fprintf (dump_file, "\n\\end{document}\n");

    char command [kCommandTerminalDumpTexLen] = "";

    sprintf (command, "pdflatex %s >/dev/null", file_name);
    if (system (command) == -1)
    {
        return kCantMakePdfTexDiff;
    }

    return kDoneDiff;
}

enum DiffError PrintAfterReadTreeDiff (const char* const input_file_name, const node_t* const root, FILE* const dump_file)
{
    ASSERT (input_file_name != NULL, "Invalid argument Input file name = %p\n",      input_file_name);
    ASSERT (root            != NULL, "Invalid argument root = %p\n",                 root);
    ASSERT (dump_file       != NULL, "Invalid argument Pointer on dump file = %p\n", dump_file);

    fprintf (dump_file, "Ну, и что же находилось в файле \"%s\"? \n"
                        "Похоже, это функция...\n", input_file_name);

    return PrintFormula (root, dump_file);
}

enum DiffError PrintSimplify (const node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",                 root);
    ASSERT (dump_file != NULL, "Invalid argument Pointer on dump file = %p\n", dump_file);

    fprintf (dump_file, "\n\\newline\n"
                        "И так, попробуем упростить эту штуковину...\n");

    return PrintFormula (root, dump_file);
}

enum DiffError PrintLoseSimplification (FILE* const dump_file)
{
    ASSERT (dump_file != NULL, "Invalid argument Pointer on dump file = %p\n", dump_file);

    fprintf (dump_file, "\n\\newline\n"
                        "Увы, похоже, это несократимая хрень...\n"
                        "Да и ладно, методом скипа идём дальше\n"
                        "\\newline\n");

    return kDoneDiff;
}

enum DiffError PrintObviouslyStart (const node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",                 root);
    ASSERT (dump_file != NULL, "Invalid argument Pointer on dump file = %p\n", dump_file);

    fprintf (dump_file, "Ну, тут опытный советский глаз сразу заметит, что\n"
                        "\\[\n");
    return PrintTreeDiffTex (root, dump_file);
}

enum DiffError PrintObviouslyEnd (const node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",                 root);
    ASSERT (dump_file != NULL, "Invalid argument Pointer on dump file = %p\n", dump_file);

    enum DiffError result = kDoneDiff;

    fprintf (dump_file, "\n=\n");
    result = PrintTreeDiffTex (root, dump_file);
    fprintf (dump_file, "\n\\]\n");

    return result;
}

enum DiffError PrintSimplificationEnd (const node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",                 root);
    ASSERT (dump_file != NULL, "Invalid argument Pointer on dump file = %p\n", dump_file);

    fprintf (dump_file, "\n"
                        "После этого преобразования очевидно, что формула приняла вид...\n");

    return PrintFormula (root, dump_file);
}


enum DiffError PrintDifferencing (const node_t* const old_root, const node_t* const new_root,
                                  FILE* const dump_file)
{
    ASSERT (dump_file != NULL, "Invalid argument Pointer on dump file = %p\n", dump_file);

    fprintf (dump_file, "\nА теперь магическим образом берём производную от этой фигни\n"
                        "\\newline\n");

    PrintFormula (old_root, dump_file);

    fprintf (dump_file, "\nОчевидно, она равна этому\n"
                        "\\newline\n");

    PrintFormula (new_root, dump_file);

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

    #define RESULT_HANDLER(result)  \
    if (result == kDoneDiff)        \
    {                               \
        return result;              \
    }

    enum DiffError result = kUndefFuncPrintTexDiff;

    if (root->type != kFunc)
    {
        return kInvalidNodeForPrintTexDiff;
    }


    result = PrintBinaryInfixFunc (root, kAdd, "+",     dump_file);
    RESULT_HANDLER (result);
    result = PrintBinaryInfixFunc (root, kSub, "-",     dump_file);
    RESULT_HANDLER (result);
    result = PrintBinaryInfixFunc (root, kMul, "\\cdot", dump_file);
    RESULT_HANDLER (result);
    result = PrintBinaryInfixFunc (root, kPow, "^",     dump_file);
    RESULT_HANDLER (result);

    result = PrintBinaryPrefixFunc (root, kDiv, "\\frac", dump_file);
    RESULT_HANDLER (result);
    result = PrintBinaryPrefixFunc (root, kLog, "\\log_", dump_file);
    RESULT_HANDLER (result);


    result = PrintUnaryFunc (root, kSin, "\\sin", dump_file);
    RESULT_HANDLER (result);
    result = PrintUnaryFunc (root, kCos, "\\cos", dump_file);
    RESULT_HANDLER (result);
    result = PrintUnaryFunc (root, kTg , "\\tan", dump_file);
    RESULT_HANDLER (result);
    result = PrintUnaryFunc (root, kCtg, "\\cot", dump_file);
    RESULT_HANDLER (result);

    result = PrintUnaryFunc (root, kLn,  "\\ln" , dump_file);
    RESULT_HANDLER (result);

    #undef RESULT_HANDLER

    return result;
}

static enum DiffError PrintBinaryInfixFunc (const node_t* const root, const enum FuncType func_type,
                                            const char* const str_func, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",      root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);
    ASSERT (str_func  != NULL, "Invalid argument str_func = %p\n",  str_func);

    enum DiffError result = kDoneDiff;

    #define ERROR_HANDLER(result)   \
    if (result != kDoneDiff)        \
    {                               \
        return result;              \
    }

    if (root->value.function == func_type)
    {
        fprintf (dump_file, " {");

        if (((func_type == kMul) || (func_type == kPow))
            && ((root->left->left != NULL) || (root->left->right != NULL)))
        {
            fprintf (dump_file, "(");
            result = PrintTreeDiffTex (root->left, dump_file);
            fprintf (dump_file, ")");
        }
        else
        {
            result = PrintTreeDiffTex (root->left, dump_file);
        }
        ERROR_HANDLER (result);

        fprintf (dump_file, "} ");

        fprintf (dump_file, "%s", str_func);

        fprintf (dump_file, " {");

        if (((func_type == kMul) || (func_type == kPow))
            && ((root->right->left != NULL) || (root->right->right != NULL)))
        {
            fprintf (dump_file, "(");
            result = PrintTreeDiffTex (root->right, dump_file);
            fprintf (dump_file, ")");
        }
        else
        {
            result = PrintTreeDiffTex (root->right, dump_file);
        }
        ERROR_HANDLER (result);

        fprintf (dump_file, "} ");

        return result;
    }

    #undef ERROR_HANDLER

    return kCantPrintBinaryInfixFunc;
}

static enum DiffError PrintBinaryPrefixFunc (const node_t* const root, const enum FuncType func_type,
                                            const char* const str_func, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",      root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);
    ASSERT (str_func  != NULL, "Invalid argument str_func = %p\n",  str_func);

    enum DiffError result = kDoneDiff;

    if (root->value.function == func_type)
    {
        fprintf (dump_file, "%s", str_func);

        fprintf (dump_file, " {");
        result = PrintTreeDiffTex (root->left,  dump_file);
        fprintf (dump_file, "} ");

        if (result != kDoneDiff)
        {
            return result;
        }

        fprintf (dump_file, " {");
        result = PrintTreeDiffTex (root->right, dump_file);
        fprintf (dump_file, "} ");
        return result;
    }

    return kCantPrintBinaryPrefixFunc;
}

static enum DiffError PrintUnaryFunc (const node_t* const root, const enum FuncType func_type,
                                            const char* const str_func, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",      root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);
    ASSERT (str_func  != NULL, "Invalid argument str_func = %p\n",  str_func);

    enum DiffError result = kDoneDiff;

    if (root->value.function == func_type)
    {
        fprintf (dump_file, "%s", str_func);

        fprintf (dump_file, " {");
        if ((root->right->left != NULL) || (root->right->right != NULL))
        {
            fprintf (dump_file, "(");
            result = PrintTreeDiffTex (root->right, dump_file);
            fprintf (dump_file, ")");
        }
        else
        {
            result = PrintTreeDiffTex (root->right, dump_file);
        }
        fprintf (dump_file, "} ");

        return result;
    }

    return kCantPrintUnaryFunc;
}

static enum DiffError PrintNumVarNodeDiff (const node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n",      root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);

    if (root->type == kNum)
    {
        fprintf (dump_file, " %.3lg ", root->value.number);

        return kDoneDiff;
    }

    if (root->type == kVar)
    {
        fprintf (dump_file, " %c ", root->value.variable);

        return kDoneDiff;
    }

    return kInvalidNodeForPrintTexDiff;
}

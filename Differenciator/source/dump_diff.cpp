#include "../differenciator.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../My_lib/Assert/my_assert.h"
#include "../../My_lib/Logger/logging.h"
#include "../../My_lib/My_stdio/my_stdio.h"

static enum DiffError PrintNodeInfoDiff  (node_t* const node, FILE* const dump_file);
static enum DiffError PrintTreeInfoDiff  (node_t* const node, FILE* const dump_file);
static enum DiffError PrintEdgesTreeDiff (node_t* const root, FILE* const dump_file);
static const char* EnumFuncToStr         (const enum FuncType func);

enum DiffError DumpDiff (node_t* const root)
{
    ASSERT (root != NULL, "Invalid argument: root = %p\n", root);

    LOG (DEBUG, "Function got arguments:\n"
                "| root = %p |\n",
                root);

    static size_t counter_dump = 0;

    FILE* html_dump_file = NULL;
    FOPEN (html_dump_file, "Dump_Files/Dump.html", "a");
    if (html_dump_file == NULL)
    {
        return kCantDumpDiff;
    }

    if (ftell (html_dump_file) == 0)
    {
        fprintf (html_dump_file, "<pre>\n<HR>\n");
    }

    FILE* dump_file = NULL;
    FOPEN (dump_file, "Dump_Files/Dump.dot", "w");
    if (dump_file == NULL)
    {
        FCLOSE (html_dump_file);
        return kCantDumpDiff;
    }

    fprintf (dump_file, "digraph\n{\n"
                        "\tfontname = \"Helvetica,Arial,sans-serif\";\n"
                        "\tnode [fontname = \"Helvetica,Arial,sans-serif\"];\n"
                        "\tgraph [rankdir = \"TB\"];\n"
                        "\tranksep = 1.5;\n\n");

    PrintTreeInfoDiff (root, dump_file);

    PrintEdgesTreeDiff (root, dump_file);

    fputc ('}', dump_file);

    FCLOSE (dump_file);

    counter_dump++;

    char command [kCommandTerminalDumpLen] = "";

    sprintf (command, "dot -Tsvg Dump_Files/Dump.dot -o Dump_Files/Dump_%lu_.svg\n", counter_dump);

    if (system (command) == -1)
    {
        FCLOSE (html_dump_file);
        return kCantDumpDiff;
    }

    fprintf (html_dump_file, "<img src = \"Dump_%lu_.svg\" width = 1450>\n<HR>\n", counter_dump);

    FCLOSE (html_dump_file);

    return kDoneDiff;
}

static enum DiffError PrintNodeInfoDiff (node_t* const node, FILE* const dump_file)
{
    ASSERT (node      != NULL, "Invalid argument node = %p\n", node);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);

//-------------------------------------------------------------------
    #define PRINT_NODE(type, color_node, data, specifier)           \
        case type:                                                  \
        {                                                           \
            fprintf (dump_file, "\t\"node%p\"\n\t[\n"               \
                                "\t\tlabel = \""                    \
                                "{ <parent> parent = %p |\n"        \
                                " node = %p |"                      \
                                " <f0> " specifier " |"             \
                                "{ <f1> left = %p |"                \
                                " <f2> right = %p } }\"\n"          \
                                "\t\tshape = \"record\"\n"          \
                                "\t\tstyle = \"filled\"\n"          \
                                "\t\tfillcolor = \"%s\"\n"          \
                                "\t\tcolor = \"%s\"\n"              \
                                "\t];\n\n",                         \
                                (void*) node, (void*) node->parent, \
                                (void*) node, data,                 \
                                (void*) node->left,                 \
                                (void*) node->right,                \
                                color_node, color_node);            \
            return kDoneDiff;                                       \
        }
//-------------------------------------------------------------------

    LOG (DEBUG, "Function got arguments:\n"
                "| node = %p | dump_file = %p |\n",
                node, dump_file);

    switch (node->type)
    {
        PRINT_NODE (kNum,  kColorNodeNum,  node->value.number,   "%.3lf");
        PRINT_NODE (kVar,  kColorNodeVar,  node->value.variable, "%c" );
        PRINT_NODE (kFunc, kColorNodeFunc, EnumFuncToStr (node->value.function), "%s" );

        case kNewNode:
            return kInvalidNodeType;

        default:
            return kInvalidNodeType;
    }

    #undef PRINT_NODE

    return kDoneDiff;
}

static enum DiffError PrintTreeInfoDiff (node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n", root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);

    LOG (DEBUG, "Function got arguments:\n"
                "| root = %p | dump_file = %p |\n",
                root, dump_file);

    PrintNodeInfoDiff (root, dump_file);

    if (root->left != NULL)
    {
        PrintTreeInfoDiff (root->left, dump_file);
    }

    if (root->right != NULL)
    {
        PrintTreeInfoDiff (root->right, dump_file);
    }

    return kDoneDiff;
}

static enum DiffError PrintEdgesTreeDiff (node_t* const root, FILE* const dump_file)
{
    ASSERT (root      != NULL, "Invalid argument root = %p\n", root);
    ASSERT (dump_file != NULL, "Invalid argument dump_file = %p\n", dump_file);

    LOG (DEBUG, "Function got arguments:\n"
                "| root = %p | dump_file = %p |\n",
                root, dump_file);

    if (root->left != NULL)
    {
        fprintf (dump_file, "\t\"node%p\":f1 -> \"node%p\""
                            "[color = \"black\"];\n\n",
                            (void*) root, (void*) root->left);
        PrintEdgesTreeDiff (root->left, dump_file);
    }

    if (root->right != NULL)
    {
        fprintf (dump_file, "\t\"node%p\":f2 -> \"node%p\""
                            "[color = \"black\"];\n\n",
                            (void*) root, (void*) root->right);
        PrintEdgesTreeDiff (root->right, dump_file);
    }

    if (root->parent != NULL)
    {
        fprintf (dump_file, "\t\"node%p\":parent -> \"node%p\" "
                            "[color = \"black\"];\n\n",
                            (void*) root, (void*) root->parent);
    }

    return kDoneDiff;
}

static const char* EnumFuncToStr (const enum FuncType func)
{
    switch (func)
    {
        case kAdd: return "+";
        case kSub: return "-";
        case kMul: return "*";
        case kDiv: return "/";
        case kPow: return "^";

        case kSin: return "sin";
        case kCos: return "cos";
        case kTg:  return "tg";
        case kCtg: return "ctg";

        case kLn:  return "ln";
        case kLog: return "log";

        case kInvalidFunc: return "Invalid function";

        default:
            return "Invalid enum element";
    }

}

#include "../differenciator.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../../My_lib/Assert/my_assert.h"
#include "../../My_lib/Logger/logging.h"
#include "../../My_lib/My_stdio/my_stdio.h"
#include "../../My_lib/helpful.h"

static enum DiffError GetAddSub (char* const input_buf, size_t* const offset, node_t** const node);
static enum DiffError GetMulDiv (char* const input_buf, size_t* const offset, node_t** const node);
static enum DiffError GetBrace  (char* const input_buf, size_t* const offset, node_t** const node);
static enum DiffError GetNumVar (char* const input_buf, size_t* const offset, node_t** const node);
static enum FuncType StrToFunc (char* const func);

enum DiffError ReadDataBase (char* const input_file_name, node_t** const root)
{
    ASSERT (input_file_name != NULL, "Invalid argument input file name = %p\n", input_file_name);
    ASSERT (root            != NULL, "Invalid argument root = %p\n", root);

    enum DiffError result = kDoneDiff;

    FILE* input = NULL;
    FOPEN (input, input_file_name, "r");
    if (input == NULL)
    {
        return kCantOpenDataBase;
    }

    size_t file_size = size_of_file (input);

    LOG (DEBUG, "Size of file = %lu\n",
                file_size);

    if (file_size == 0)
    {
        return kCantReadDataBase;
    }

    char* input_buf = (char*) calloc (file_size + 1, sizeof (char));
    if (input_buf == NULL)
    {
        return kCantCallocInputBuffer;
    }

    LOG (DEBUG, "Input buffer = %p\n"
                "Size of file = %lu\n",
                input_buf, file_size);

    if (fread (input_buf, sizeof (char), file_size, input) == 0)
    {
        FREE_AND_NULL (input_buf);
        return kCantReadDataBase;
    }

    input_buf [file_size] = '\0';

    size_t offset = 0;

    result = GetAddSub (input_buf, &offset, root);

    FREE_AND_NULL (input_buf);

    return result;
}

static enum DiffError GetNumVar (char* const input_buf, size_t* const offset, node_t** const node)
{
    ASSERT (input_buf != NULL, "Invalid argument input_buf = %p\n", input_buf);
    ASSERT (offset    != NULL, "Invalid argument offset = %p\n",    offset);
    ASSERT (node      != NULL, "Invalid argument node = %p\n",      node);

    enum DiffError result = kDoneDiff;

    LOG (DEBUG, "Input buffer    = %p\n"
                "Offset          = %lu\n"
                "Node*           = %p\n"
                "Run time symbol = {%c}\n",
                input_buf, *offset, node, input_buf [*offset]);

    *offset += skip_space_symbols (input_buf + *offset);

    if (*node == NULL)
    {
        *node = FuncCtor ();
    }

    if (isalpha (input_buf [*offset]))
    {
        if (isalpha (input_buf [*offset + 1]))
        {
            char func   [kLenFunc]   = "";

            sscanf (input_buf + *offset, "%[^ ^\t^\n^(^)]", func);

            LOG (DEBUG, "Read function = %s\n", func);

            (*node)->type = kFunc;
            (*node)->value.function = StrToFunc (func);

            *offset += strlen (func);
            *offset += skip_space_symbols (input_buf + *offset);

            result = GetBrace (input_buf, offset, &((*node)->right));

            return result;
        }
        else
        {
            (*node)->type = kVar;
            (*node)->value.variable = input_buf [*offset];

            (*offset)++;
            *offset += skip_space_symbols (input_buf + *offset);

            return result;
        }
    }

    double number = 0;
    if (sscanf (input_buf + *offset, "%lf", &number) == 1)
    {
        char str_num [kLenNumber] = "";
        char format  [kLenFormat] = "";

        sprintf (format,    "%%%lu[0-9.]", kLenNumber - 1);

        LOG (DEBUG, "Format for number = '%s'\n", format);

        sscanf  (input_buf + *offset, format,   str_num);

        LOG (DEBUG, "Read number = %lf\n"
                    "Str_num = %s\n"
                    "Offset = %lu\n", number, str_num, *offset + 1);

        (*node)->type = kNum;
        (*node)->value.number = number;

        *offset += strlen (str_num);
    }

    *offset += skip_space_symbols (input_buf + *offset);

    return result;
}

static enum DiffError GetBrace (char* const input_buf, size_t* const offset, node_t** const node)
{
    ASSERT (input_buf != NULL, "Invalid argument input_buf = %p\n", input_buf);
    ASSERT (offset    != NULL, "Invalid argument offset = %p\n",    offset);
    ASSERT (node      != NULL, "Invalid argument node = %p\n",      node);

    enum DiffError result = kDoneDiff;

    LOG (DEBUG, "Input buffer    = %p\n"
                "Offset          = %lu\n"
                "Node*           = %p\n"
                "Run time symbol = {%c}\n",
                input_buf, *offset, node, input_buf [*offset]);

    *offset += skip_space_symbols (input_buf + *offset);

    if (input_buf [*offset] == '(')
    {
        (*offset)++;

        result = GetAddSub (input_buf, offset, node);
        if (result != kDoneDiff)
        {
            return result;
        }

        if (input_buf [*offset] != ')')
        {
            return kNoBraceCloser;
        }

        (*offset)++;
        *offset += skip_space_symbols (input_buf + *offset);

        return result;
    }

    *offset += skip_space_symbols (input_buf + *offset);

    return GetNumVar (input_buf, offset, node);
}

static enum DiffError GetMulDiv (char* const input_buf, size_t* const offset, node_t** const node)
{
    ASSERT (input_buf != NULL, "Invalid argument input_buf = %p\n", input_buf);
    ASSERT (offset    != NULL, "Invalid argument offset = %p\n",    offset);
    ASSERT (node      != NULL, "Invalid argument node = %p\n",      node);

    enum DiffError result = kDoneDiff;

    LOG (DEBUG, "Input buffer    = %p\n"
                "Offset          = %lu\n"
                "Node*           = %p\n"
                "Run time symbol = {%c}\n",
                input_buf, *offset, node, input_buf [*offset]);

    *offset += skip_space_symbols (input_buf + *offset);

    result = GetBrace (input_buf, offset, node);

    *offset += skip_space_symbols (input_buf + *offset);

    LOG (DEBUG, "Run time symbol = {%c}\n", input_buf [*offset]);

    if ((input_buf [*offset] != '*')
     && (input_buf [*offset] != '/'))
    {
        return result;
    }

    node_t* root = FuncCtor ();

    root->left = *node;
    (*node)->parent = root;
    *node = root;

    while ((input_buf [*offset] == '*')
        || (input_buf [*offset] == '/'))
    {
        root->type = kFunc;

        if (input_buf [(*offset)++] == '*')
        {
            root->value.function = kMul;
        }
        else
        {
            root->value.function = kDiv;
        }

        result = GetBrace (input_buf, offset, &(root->right));

        root->right->parent = root;

        if (input_buf [*offset] == '\0')
        {
            return kDoneDiff;
        }
    }

    *offset += skip_space_symbols (input_buf + *offset);

    return result;
}

static enum DiffError GetAddSub (char* const input_buf, size_t* const offset, node_t** const node)
{
    ASSERT (input_buf != NULL, "Invalid argument input_buf = %p\n", input_buf);
    ASSERT (offset    != NULL, "Invalid argument offset = %p\n",    offset);
    ASSERT (node      != NULL, "Invalid argument node = %p\n",      node);

    enum DiffError result = kDoneDiff;

    LOG (DEBUG, "Input buffer    = %p\n"
                "Offset          = %lu\n"
                "Node*           = %p\n"
                "Run time symbol = {%c}\n",
                input_buf, *offset, node, input_buf [*offset]);

    *offset += skip_space_symbols (input_buf + *offset);

    result = GetMulDiv (input_buf, offset, node);

    LOG (DEBUG, "Run time symbol = {%c}\n", input_buf [*offset]);

    *offset += skip_space_symbols (input_buf + *offset);

    if ((input_buf [*offset] != '+')
     && (input_buf [*offset] != '-'))
    {
        return result;
    }

    node_t* root = FuncCtor ();

    root->left = *node;
    (*node)->parent = root;
    *node = root;

    while ((input_buf [*offset] == '+')
        || (input_buf [*offset] == '-'))
    {
        root->type = kFunc;

        if (input_buf [(*offset)++] == '+')
        {
            root->value.function = kAdd;
        }
        else
        {
            root->value.function = kSub;
        }

        result = GetMulDiv (input_buf, offset, &(root->right));

        root->right->parent = root;

        if (input_buf [*offset] == '\0')
        {
            return kDoneDiff;
        }
    }

    *offset += skip_space_symbols (input_buf + *offset);

    return result;
}

static enum FuncType StrToFunc (char* const func)
{
    ASSERT (func != NULL, "Invalid argument func = %p\n", func);

    #define FUNC_TYPE_DETECTOR(func_type, func_str)             \
        if (my_strcmp (func, func_str) == 0)                    \
        {                                                       \
            return func_type;                                   \
        }

    FUNC_TYPE_DETECTOR (kAdd, "+");
    FUNC_TYPE_DETECTOR (kSub, "-");
    FUNC_TYPE_DETECTOR (kMul, "*");
    FUNC_TYPE_DETECTOR (kDiv, "/");

    FUNC_TYPE_DETECTOR (kSin, "sin");
    FUNC_TYPE_DETECTOR (kCos, "cos");
    FUNC_TYPE_DETECTOR (kTg,  "tg");
    FUNC_TYPE_DETECTOR (kCtg, "ctg");

    #undef FUNC_TYPE_DETECTOR

    return kInvalidFunc;
}


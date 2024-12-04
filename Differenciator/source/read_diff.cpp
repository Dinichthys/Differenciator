#include "../differenciator.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../../My_lib/Assert/my_assert.h"
#include "../../My_lib/Logger/logging.h"
#include "../../My_lib/My_stdio/my_stdio.h"
#include "../../My_lib/helpful.h"

static enum DiffError GetAddSub   (char* const input_buf, size_t* const offset, node_t** const node);
static enum DiffError GetMulDiv   (char* const input_buf, size_t* const offset, node_t** const node);
static enum DiffError GetPow      (char* const input_buf, size_t* const offset, node_t** const node);
static enum DiffError GetBrace    (char* const input_buf, size_t* const offset, node_t** const node);
static enum DiffError GetNumVar   (char* const input_buf, size_t* const offset, node_t** const node);
static enum DiffError SyntaxError (const size_t offset);
static enum FuncType  StrToFunc   (char* const func);
static void           SkipNumber  (char* const input_buf, size_t* const offset);

enum DiffError ReadDataBase (const char* const input_file_name, node_t** const root)
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

    offset += skip_space_symbols (input_buf + offset);

    if (input_buf [offset] != '\0')
    {
        FREE_AND_NULL (input_buf);

        return SyntaxError (offset);
    }

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

            if ((*node)->value.function == kInvalidFunc)
            {
                return kUndefinedFuncForRead;
            }

            *offset += strlen (func);
            *offset += skip_space_symbols (input_buf + *offset);

            result = GetBrace (input_buf, offset, &((*node)->right));

            ConnectTree (*node);

            *offset += skip_space_symbols (input_buf + *offset);

            if ((*node)->value.function != kLog)
            {
                return result;
            }

            (*node)->left = (*node)->right;

            (*node)->right = NULL;

            result = GetBrace (input_buf, offset, &((*node)->right));

            ConnectTree (*node);

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
        LOG (DEBUG, "Read number = %lf\n"
                    "Offset = %lu\n"
                    "Node** = %p\n", number, *offset + 1, node);

        (*node)->type = kNum;
        (*node)->value.number = number;

        SkipNumber (input_buf, offset);
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

static enum DiffError GetPow (char* const input_buf, size_t* const offset, node_t** const node)
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

    LOG (DEBUG, "Run time symbol = {%c}\n", input_buf [*offset]);

    *offset += skip_space_symbols (input_buf + *offset);

    if (input_buf [*offset] != '^')
    {
        return result;
    }

    node_t* root = FuncCtor ();

    root->left = *node;
    (*node)->parent = root;
    *node = root;

    while (input_buf [(*offset)++] == '^')
    {
        if (root->type != kNewNode)
        {
            node_t* subtree = root;
            root = FuncCtor ();
            root->left = subtree;
            root->left->parent = root;
        }

        root->type = kFunc;

        root->value.function = kPow;

        result = GetBrace (input_buf, offset, &(root->right));

        *offset += skip_space_symbols (input_buf + *offset);

        root->right->parent = root;

        *node = root;

        if (input_buf [*offset] == '\0')
        {
            return kDoneDiff;
        }
    }

    *offset += skip_space_symbols (input_buf + *offset);

    return result;
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

    result = GetPow (input_buf, offset, node);

    LOG (DEBUG, "Run time symbol = {%c}\n", input_buf [*offset]);

    *offset += skip_space_symbols (input_buf + *offset);

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
        if (root->type != kNewNode)
        {
            node_t* subtree = root;
            root = FuncCtor ();
            root->left = subtree;
            root->left->parent = root;
        }

        root->type = kFunc;

        if (input_buf [(*offset)++] == '*')
        {
            root->value.function = kMul;
        }
        else
        {
            root->value.function = kDiv;
        }

        result = GetPow (input_buf, offset, &(root->right));

        *offset += skip_space_symbols (input_buf + *offset);

        root->right->parent = root;

        *node = root;

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

    if (input_buf [*offset] == '-')
    {
        if (*node != NULL)
        {
            FuncDtor (*node);
        }
        node_t* root = FuncCtor ();
        root->type = kFunc;
        root->value.function = kSub;

        LOG (DEBUG, "Root with minus = %p\n", root);

        root->left = FuncCtor ();
        root->left->parent = root;
        root->left->type = kNum;
        root->left->value.number = 0;
        *node = root;

        (*offset)++;

        result = GetMulDiv (input_buf, offset, &((*node)->right));
    }
    else
    {
        result = GetMulDiv (input_buf, offset, node);
    }

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
        if (root->type != kNewNode)
        {
            node_t* subtree = root;
            root = FuncCtor ();
            root->left = subtree;
            root->left->parent = root;
        }

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

        *offset += skip_space_symbols (input_buf + *offset);

        root->right->parent = root;

        *node = root;

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
    FUNC_TYPE_DETECTOR (kPow, "^");

    FUNC_TYPE_DETECTOR (kSin, "sin");
    FUNC_TYPE_DETECTOR (kCos, "cos");
    FUNC_TYPE_DETECTOR (kTg,  "tg" );
    FUNC_TYPE_DETECTOR (kCtg, "ctg");
    FUNC_TYPE_DETECTOR (kLn,  "ln" );
    FUNC_TYPE_DETECTOR (kLog, "log");

    #undef FUNC_TYPE_DETECTOR

    return kInvalidFunc;
}

static void SkipNumber (char* const input_buf, size_t* const offset)
{
    ASSERT (input_buf != NULL, "Invalid argument input_buf = %p\n", input_buf);
    ASSERT (offset    != NULL, "Invalid argument offset = %p\n",    offset);

    *offset += skip_space_symbols (input_buf + *offset);

    if (input_buf [*offset] == '-')
    {
        (*offset)++;
    }

    *offset += skip_space_symbols (input_buf + *offset);

    while (isdigit (input_buf [*offset]))
    {
        (*offset)++;
    }

    if (input_buf [*offset] == '.')
    {
        (*offset)++;
    }

    while (isdigit (input_buf [*offset]))
    {
        (*offset)++;
    }

    *offset += skip_space_symbols (input_buf + *offset);
}

static enum DiffError SyntaxError (const size_t offset)
{
    fprintf (stderr, "Invalid syntax with offset = %lu\n", offset);

    return kSyntaxError;
}

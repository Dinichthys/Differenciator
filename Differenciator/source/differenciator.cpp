#include "../differenciator.h"

#include <stdlib.h>
#include <string.h>

#include "../../My_lib/Assert/my_assert.h"
#include "../../My_lib/Logger/logging.h"
#include "../../My_lib/My_stdio/my_stdio.h"
#include "../../My_lib/helpful.h"

// static enum DiffError ReadDataBaseFromFile (node_t* const node, char* const input_buf, size_t* offset);
// static enum DiffError ReadNodeValue        (node_t* const node, char* const input_buf, size_t* offset);

node_t* FuncCtor (void)
{
    node_t* root = (node_t*) calloc (1, sizeof (node_t));

    if (root == NULL)
    {
        return NULL;
    }

    root->type = kNewNode;

    root->parent = root->left = root->right = NULL;

    return root;
}

enum DiffError FuncDtor (node_t* const root)
{
    ASSERT (root != NULL, "Invalid argument root = %p\n", root);

    if (root->left != NULL)
    {
        FuncDtor (root->left);
    }

    if (root->right != NULL)
    {
        FuncDtor (root->right);
    }

    free (root);

    return kDoneDiff;
}

node_t* AddNode (const node_t set_val)
{
    node_t* node = NULL;

    node = (node_t*) calloc (1, sizeof (node_t));
    if (node == NULL)
    {
        return NULL;
    }
    *(node) = set_val;

    return node;
}
//
// //ОТЛОЖИЛ ДО ЛУЧШИХ ВРЕМЁН -
// enum DiffError ReadDataBase (node_t* const root, const char file_name[])
// {
//     ASSERT (root      != NULL, "Invalid argument root = %p\n", root);
//     ASSERT (file_name != NULL, "Invalid argument file_name = %p\n", file_name);
//
//     FILE* input = NULL;
//     FOPEN (input, file_name, "r");
//     if (input == NULL)
//     {
//         return kCantReadDataBase;
//     }
//
//     size_t size_buf = size_of_file (input);
//     if (size_buf == 0)
//     {
//         return kCantReadDataBase;
//     }
//
//     char* input_buf = (char*) calloc (size_buf, sizeof (char));
//     if (input_buf == NULL)
//     {
//         return kCantReadDataBase;
//     }
//
//     if (fread (input_buf, sizeof (char), size_buf, input) == 0)
//     {
//         FREE_AND_NULL (input_buf);
//         return kCantReadDataBase;
//     }
//     FCLOSE (input);
//
//     size_t offset = 0;
//
//     enum DiffError result = ReadDataBaseFromFile (root, input_buf, &offset);
//
//     FREE_AND_NULL (input_buf);
//
//     return result;
// }

const char* EnumErrorToStr (const enum DiffError error)
{
    #define CASE(error)             \
        case error:                 \
        {                           \
            return #error;          \
        }

    switch (error)
    {
        CASE (kDoneDiff);

        CASE (kCantAddNode);
        CASE (kCantCreateFunc);

        CASE (kCantReadDataBase);
        CASE (kCantCallocInputBuffer);
        CASE (kCantOpenDataBase);
        CASE (kNoBraceCloser);

        CASE (kCantDumpDiff);
        CASE (kInvalidNodeType);

        default:
            return "Invalid enum element";
    }

    #undef CASE
}
//
// // ОТЛОЖИЛ ДО ЛУЧШИХ ВРЕМЁН -  Сделай читалку. а то она не работает пока что
// static enum DiffError ReadDataBaseFromFile (node_t* const node, char* const input_buf, size_t* offset)
// {
//     ASSERT (node      != NULL, "Invalid argument node = %p\n", node);
//     ASSERT (input_buf != NULL, "Invalid argument input buffer = %p\n", input_buf);
//     ASSERT (offset    != NULL, "Invalid argument offset = %p\n", offset);
//
//     enum DiffError result = kDoneDiff;
//
//     char check_symbol = '\0';
//
//     sscanf (input_buf + *offset, "%c", &check_symbol);
//
//     LOG (DEBUG, "Check symbol (should open break) = %c\n", check_symbol);
//
//     if (check_symbol == '(')
//     {
//         node_t new_node = {};
//         new_node.type = kNewNode;
//         new_node.parent = node;
//         new_node.left = new_node.right = NULL;
//
//         node->left = AddNode (new_node);
//         node->right = AddNode (new_node);
//
//         *offset = strchr (input_buf + *offset, check_symbol) - input_buf + sizeof (check_symbol);
//
//         result = ReadDataBaseFromFile (node->left, input_buf, offset);
//
//         if (result != kDoneDiff)
//         {
//             return kCantReadDataBase;
//         }
//
//         result = ReadNodeValue (node, input_buf, offset);
//
//         *offset = strchr (input_buf + *offset, check_symbol) - input_buf;
//
//         if (result != kDoneDiff)
//         {
//             return kCantReadDataBase;
//         }
//
//         result = ReadDataBaseFromFile (node->right, input_buf, offset);
//
//         if (result != kDoneDiff)
//         {
//             return kCantReadDataBase;
//         }
//
//         sscanf (input_buf + *offset, "%c", &check_symbol);
//
//         LOG (DEBUG, "Check symbol (should close break) = %c\n", check_symbol);
//
//         if (check_symbol != ')')
//         {
//             return kCantReadDataBase;
//         }
//
//         *offset = strchr (input_buf + *offset, ')') - input_buf + 1;
//
//         return kDoneDiff;
//     }
//
//     result = ReadNodeValue (node, input_buf, offset);
//
//     *offset = strchr (input_buf + *offset, ')') - input_buf + 1;
//
//     return result;
// }
//
// // КАРАНТИННАЯ ЗОНА - статики
//
// static enum DiffError ReadNodeValue (node_t* const node, char* const input_buf, size_t* offset)
// {
//     ASSERT (node      != NULL, "Invalid argument node = %p\n", node);
//     ASSERT (input_buf != NULL, "Invalid argument input buffer = %p\n", input_buf);
//     ASSERT (offset    != NULL, "Invalid argument offset = %p\n", offset);
//
//     double number = 0;
//
//     LOG (DEBUG, "Offset = %lu\n", *offset);
//
//     if (sscanf (input_buf + *offset, "%lf", &number) == 1)
//     {
//         char str_num [kLenNumber] = "";
//         char format  [kLenFormat] = "";
//
//         sprintf (format,    "%%%lu[0-9.]", kLenNumber - 1);
//
//         LOG (DEBUG, "Format for number = '%s'\n", format);
//
//         sscanf  (input_buf + *offset, format,   str_num);
//
//         LOG (DEBUG, "Read number = %lf\n"
//                     "Str_num = %s\n"
//                     "Offset = %lu\n", number, str_num, *offset + 1);
//
//         node->type = kNum;
//         node->value.number = number;
//
//         *offset += strlen (str_num);
//
//         return kDoneDiff;
//     }
//
//     char func   [kLenFunc]   = "";
//
//     if (sscanf (input_buf + *offset, "%[^ ^\t^\n^(^)]", func) != 0)
//     {
//         if ((strlen (func) == 1) && (func [0] >= 'a') && (func [0] <= 'z'))
//         {
//             LOG (DEBUG, "Read variable = %c\n", func [0]);
//
//             node->type = kVar;
//             node->value.variable = func [0];
//         }
//         else
//         {
//             LOG (DEBUG, "Read function = %s\n", func);
//
//             node->type = kFunc;
//             node->value.function = StrToFunc (func);
//         }
//
//         return kDoneDiff;
//     }
//
//     return kCantReadDataBase;
// }

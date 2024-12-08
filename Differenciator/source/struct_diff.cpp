#include "struct_diff.h"

#include <stdlib.h>
#include <string.h>

#include "My_lib/Assert/my_assert.h"
#include "My_lib/Logger/logging.h"
#include "My_lib/My_stdio/my_stdio.h"
#include "My_lib/helpful.h"

node_t* ExpressionCtor (void)
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

// REVIEW -  нейминг говно
enum DiffError ExpressionDtor (node_t* const root)
{
    ASSERT (root != NULL, "Invalid argument root = %p\n", root);

    root->parent = NULL;

    if (root->left != NULL)
    {
        ExpressionDtor (root->left);
    }

    if (root->right != NULL)
    {
        ExpressionDtor (root->right);
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
        CASE (kUndefinedFuncForRead);
        CASE (kSyntaxError);

        CASE (kCantDiffTree);
        CASE (kInvalidFuncForDiff);
        CASE (kUndefinedFuncForDiff);

        CASE (kCantDumpDiff);
        CASE (kInvalidNodeType);

        CASE (kCantOpenFileDumpTexDiff);
        CASE (kCantMakePdfTexDiff);
        CASE (kInvalidNodeForPrintTexDiff);
        CASE (kUndefFuncPrintTexDiff);
        CASE (kCantPrintBinaryInfixFunc);
        CASE (kCantPrintBinaryPrefixFunc);
        CASE (kCantPrintUnaryFunc);
        CASE (kInvalidModeTypeDiffError);

        default:
            return "Invalid enum element";
    }

    #undef CASE
}

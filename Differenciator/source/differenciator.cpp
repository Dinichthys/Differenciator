#include "../differenciator.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../../My_lib/Assert/my_assert.h"
#include "../../My_lib/Logger/logging.h"

static enum DiffError DiffTree (node_t** const new_root, const node_t* const root, const char var);
static enum DiffError DiffNum  (node_t** const new_root, const node_t* const root);
static enum DiffError DiffVar  (node_t** const new_root, const node_t* const root, const char var);
static enum DiffError DiffFunc (node_t** const new_root, const node_t* const root, const char var);

static enum DiffError DiffkAdd (node_t** const new_root, const node_t* const root, const char var);
static enum DiffError DiffkSub (node_t** const new_root, const node_t* const root, const char var);
static enum DiffError DiffkMul (node_t** const new_root, const node_t* const root, const char var);
static enum DiffError DiffkDiv (node_t** const new_root, const node_t* const root, const char var);
static enum DiffError DiffkPow (node_t** const new_root, const node_t* const root, const char var);

static enum DiffError DiffkSin (node_t** const new_root, const node_t* const root, const char var);
static enum DiffError DiffkCos (node_t** const new_root, const node_t* const root, const char var);
static enum DiffError DiffkTg  (node_t** const new_root, const node_t* const root, const char var);
static enum DiffError DiffkCtg (node_t** const new_root, const node_t* const root, const char var);

static enum DiffError DiffkLn  (node_t** const new_root, const node_t* const root, const char var);
static enum DiffError DiffkLog (node_t** const new_root, const node_t* const root, const char var);

enum DiffError Differencing (node_t** const new_root, node_t* const root, FILE* const dump_file)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    char var = '\0';

    fprintf (stdout, "Enter the variable by which the expression needs to be differentiated: \n");
    if (fscanf  (stdin, "%c", &var) == 0)
    {
        return kCantDiffTree;
    }

    result = DiffTree (new_root, root, var);

    PrintDifferencing (root, *new_root, dump_file);

    if (result != kDoneDiff)
    {
        if (*new_root != NULL)
        {
            FuncDtor (*new_root);
        }

        return kCantDiffTree;
    }

    FuncDtor (root);

    return kDoneDiff;
}

static enum DiffError DiffTree (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    if (root->type == kNum)
    {
        return DiffNum (new_root, root);
    }

    if (root->type == kVar)
    {
        return DiffVar (new_root, root, var);
    }

    return DiffFunc (new_root, root, var);
}

static enum DiffError DiffNum (node_t** const new_root, const node_t* const root)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    node_t diff_node = {};
    diff_node.type = kNum;
    diff_node.value.number = 0;
    diff_node.left = diff_node.right = NULL;

    *new_root = AddNode (diff_node);

    return kDoneDiff;
}

static enum DiffError DiffVar (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    if (root->value.variable != var)
    {
        return DiffNum (new_root, root);
    }

    node_t diff_node = {};
    diff_node.type = kNum;
    diff_node.value.number = 1;
    diff_node.left = diff_node.right = NULL;

    *new_root = AddNode (diff_node);

    return kDoneDiff;
}

static enum DiffError DiffFunc (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    #define CASE(function)                                  \
        case function:                                      \
        {                                                   \
            return Diff ## function (new_root, root, var);  \
        }

    switch (root->value.function)
    {
        CASE (kAdd);
        CASE (kSub);
        CASE (kMul);
        CASE (kDiv);
        CASE (kPow);

        CASE (kSin);
        CASE (kCos);
        CASE (kTg);
        CASE (kCtg);

        CASE (kLn);
        CASE (kLog);

        case kInvalidFunc:
            return kInvalidFuncForDiff;

        default:
            return kUndefinedFuncForDiff;
    }

    return kDoneDiff;
}

static enum DiffError DiffkAdd (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    *new_root = FuncCtor ();
    (*new_root)->type = kFunc;
    (*new_root)->value.function = kAdd;

    result = DiffTree (&((*new_root)->left), root->left, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DiffTree (&((*new_root)->right), root->right, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = ConnectTree (*new_root);

    return kDoneDiff;
}

static enum DiffError DiffkSub (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    *new_root = FuncCtor ();
    (*new_root)->type = kFunc;
    (*new_root)->value.function = kSub;

    result = DiffTree (&((*new_root)->left), root->left, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DiffTree (&((*new_root)->right), root->right, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = ConnectTree (*new_root);

    return kDoneDiff;
}

static enum DiffError DiffkMul (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    *new_root = FuncCtor ();
    (*new_root)->type = kFunc;
    (*new_root)->value.function = kAdd;

    (*new_root)->left = FuncCtor ();
    (*new_root)->left->type = kFunc;
    (*new_root)->left->value.function = kMul;

    (*new_root)->right = FuncCtor ();
    (*new_root)->right->type = kFunc;
    (*new_root)->right->value.function = kMul;

    result = DiffTree (&((*new_root)->left->left), root->left, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DuplicateTreeDiff (&((*new_root)->left->right), root->right);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DiffTree (&((*new_root)->right->right), root->right, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DuplicateTreeDiff (&((*new_root)->right->left), root->left);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = ConnectTree (*new_root);

    return kDoneDiff;
}

static enum DiffError DiffkDiv (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    *new_root = FuncCtor ();
    (*new_root)->type = kFunc;
    (*new_root)->value.function = kDiv;

    (*new_root)->left = FuncCtor ();
    (*new_root)->left->type = kFunc;
    (*new_root)->left->value.function = kSub;

    (*new_root)->left->left = FuncCtor ();
    (*new_root)->left->left->type = kFunc;
    (*new_root)->left->left->value.function = kMul;

    (*new_root)->left->right = FuncCtor ();
    (*new_root)->left->right->type = kFunc;
    (*new_root)->left->right->value.function = kMul;

    (*new_root)->right = FuncCtor ();
    (*new_root)->right->type = kFunc;
    (*new_root)->right->value.function = kPow;

    result = DiffTree (&((*new_root)->left->left->left), root->left, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DuplicateTreeDiff (&((*new_root)->left->left->right), root->right);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DiffTree (&((*new_root)->left->right->right), root->left, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DuplicateTreeDiff (&((*new_root)->left->right->left), root->right);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DuplicateTreeDiff (&((*new_root)->right->left), root->right);
    if (result != kDoneDiff)
    {
        return result;
    }

    (*new_root)->right->right = FuncCtor ();
    (*new_root)->right->right->type = kNum;
    (*new_root)->right->right->value.number = 2;

    result = ConnectTree (*new_root);

    return kDoneDiff;
}

static enum DiffError DiffkPow (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    *new_root = FuncCtor ();
    (*new_root)->type = kFunc;
    (*new_root)->value.function = kMul;

    result = DuplicateTreeDiff (&((*new_root)->left), root);
    if (result != kDoneDiff)
    {
        return result;
    }

    node_t* new_node = (*new_root)->right = FuncCtor ();
    new_node->type = kFunc;
    new_node->value.function = kAdd;

    new_node->left = FuncCtor ();
    new_node->left->type = kFunc;
    new_node->left->value.function = kMul;

    result = DiffTree (&(new_node->left->left), root->right, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    new_node->left->right = FuncCtor ();
    new_node->left->right->type = kFunc;
    new_node->left->right->value.function = kLn;

    result = DuplicateTreeDiff (&(new_node->left->right->right), root->left);
    if (result != kDoneDiff)
    {
        return result;
    }

    new_node->right = FuncCtor ();
    new_node->right->type = kFunc;
    new_node->right->value.function = kMul;

    new_node->right->left = FuncCtor ();
    new_node->right->left->type = kFunc;
    new_node->right->left->value.function = kDiv;

    result = DuplicateTreeDiff (&(new_node->right->left->right), root->left);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DuplicateTreeDiff (&(new_node->right->left->left), root->right);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DiffTree (&(new_node->right->right), root->left, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = ConnectTree (*new_root);

    return kDoneDiff;
}

static enum DiffError DiffkSin (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    *new_root = FuncCtor ();
    (*new_root)->type = kFunc;
    (*new_root)->value.function = kMul;

    result = DiffTree (&((*new_root)->left), root->right, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    (*new_root)->right = FuncCtor ();
    (*new_root)->right->type = kFunc;
    (*new_root)->right->value.function = kCos;

    result = DuplicateTreeDiff (&((*new_root)->right->right), root->right);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = ConnectTree (*new_root);

    return kDoneDiff;
}

static enum DiffError DiffkCos (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    *new_root = FuncCtor ();
    (*new_root)->type = kFunc;
    (*new_root)->value.function = kMul;

    (*new_root)->left = FuncCtor ();
    (*new_root)->left->type = kFunc;
    (*new_root)->left->value.function = kMul;

    (*new_root)->left->left = FuncCtor ();
    (*new_root)->left->left->type = kNum;
    (*new_root)->left->left->value.number = -1;

    result = DiffTree (&((*new_root)->left->right), root->right, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    (*new_root)->right = FuncCtor ();
    (*new_root)->right->type = kFunc;
    (*new_root)->right->value.function = kSin;

    result = DuplicateTreeDiff (&((*new_root)->right->right), root->right);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = ConnectTree (*new_root);

    return kDoneDiff;
}

static enum DiffError DiffkTg (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    *new_root = FuncCtor ();
    (*new_root)->type = kFunc;
    (*new_root)->value.function = kDiv;

    result = DiffTree (&((*new_root)->left), root->right, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    (*new_root)->right = FuncCtor ();
    (*new_root)->right->type = kFunc;
    (*new_root)->right->value.function = kPow;

    (*new_root)->right->right = FuncCtor ();
    (*new_root)->right->right->type = kNum;
    (*new_root)->right->right->value.number = 2;

    (*new_root)->right->left = FuncCtor ();
    (*new_root)->right->left->type = kFunc;
    (*new_root)->right->left->value.function = kCos;

    result = DuplicateTreeDiff (&((*new_root)->right->left->right), root->right);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = ConnectTree (*new_root);

    return kDoneDiff;
}

static enum DiffError DiffkCtg (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    *new_root = FuncCtor ();
    (*new_root)->type = kFunc;
    (*new_root)->value.function = kDiv;

    (*new_root)->left = FuncCtor ();
    (*new_root)->left->type = kFunc;
    (*new_root)->left->value.number = kMul;

    (*new_root)->left->left = FuncCtor ();
    (*new_root)->left->left->type = kNum;
    (*new_root)->left->left->value.number = -1;

    result = DiffTree (&((*new_root)->left->right), root->right, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    (*new_root)->right = FuncCtor ();
    (*new_root)->right->type = kFunc;
    (*new_root)->right->value.function = kPow;

    (*new_root)->right->right = FuncCtor ();
    (*new_root)->right->right->type = kNum;
    (*new_root)->right->right->value.number = 2;

    (*new_root)->right->left = FuncCtor ();
    (*new_root)->right->left->type = kFunc;
    (*new_root)->right->left->value.function = kSin;

    result = DuplicateTreeDiff (&((*new_root)->right->left->right), root->right);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = ConnectTree (*new_root);

    return kDoneDiff;
}

static enum DiffError DiffkLn (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    enum DiffError result = kDoneDiff;

    *new_root = FuncCtor ();
    (*new_root)->type = kFunc;
    (*new_root)->value.function = kDiv;

    result = DiffTree (&((*new_root)->left), root->right, var);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = DuplicateTreeDiff (&((*new_root)->right), root->right);
    if (result != kDoneDiff)
    {
        return result;
    }

    result = ConnectTree (*new_root);

    return kDoneDiff;
}

static enum DiffError DiffkLog (node_t** const new_root, const node_t* const root, const char var)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    node_t* temp_node = FuncCtor ();
    temp_node->type = kFunc;
    temp_node->value.function = kDiv;

    temp_node->left = FuncCtor ();
    temp_node->left->parent = temp_node;
    temp_node->left->type = kFunc;
    temp_node->left->value.function = kLn;
    temp_node->left->right = root->right;

    temp_node->right = FuncCtor ();
    temp_node->right->parent = temp_node;
    temp_node->right->type = kFunc;
    temp_node->right->value.function = kLn;
    temp_node->right->right = root->left;

    enum DiffError result = DiffkDiv (new_root, temp_node, var);

    free (temp_node->left);
    free (temp_node->right);
    free (temp_node);

    ConnectTree (*new_root);

    return result;
}

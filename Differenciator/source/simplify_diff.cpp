#include "../differenciator.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../My_lib/Assert/my_assert.h"
#include "../../My_lib/Logger/logging.h"

static node_t* SimplifyNum     (node_t* const node, const double number);
static node_t* SimplifyAddNull (node_t* const node);
static node_t* SimplifyFuncs   (node_t* const root);

// ИДУТ РАБОТЫ! НЕ ЛЕЗЬ! -
// ДЕЛО СДЕЛАНО -
node_t* Simplify (node_t* const root)
{
    ASSERT (root != NULL, "Invalid argument root = %p\n", root);

    LOG (DEBUG, "Root = %p\n", root);

    const double zero = 0;

    if (root->type != kFunc)
    {
        return root;
    }

    if (((root->value.function == kAdd)
        && ((memcmp (&(root->right->value.number), &zero, sizeof (zero)) == 0)
        ||  (memcmp (&(root->left->value.number),  &zero, sizeof (zero)) == 0)))
        ||  ((root->value.function == kSub) && (memcmp (&(root->right->value.number), &zero, sizeof (zero)) == 0)))
    {
        return SimplifyAddNull (root);
    }

    if (((root->value.function == kMul)
        && ((memcmp (&(root->right->value.number), &zero, sizeof (zero)) == 0)
        ||  (memcmp (&(root->left->value.number),  &zero, sizeof (zero)) == 0)))
        || ((root->value.function == kDiv)
        && (memcmp (&(root->left->value.number), &zero, sizeof (zero)) == 0)))
    {
        return SimplifyNum (root, 0);
    }

    if ((((root->left  != NULL) && (root->left->type  == kNum)
       && (root->right != NULL) && (root->right->type == kNum))
      || (((root->left == NULL)) && (root->right != NULL) && (root->right->type == kNum))))
    {
        return SimplifyFuncs (root);
    }

    node_t* old_left  = root->left;
    node_t* old_right = root->right;

    if (root->left != NULL)
    {
        root->left = Simplify (root->left);
    }

    if (root->right != NULL)
    {
        root->right = Simplify (root->right);
    }

    if ((old_left == root->left) && (old_right == root->right))
    {
        return root;
    }

    return Simplify (root);
}

static node_t* SimplifyNum (node_t* const node, const double number)
{
    ASSERT (node != NULL, "Invalid argument root = %p\n", node);

    node_t* const parent = node->parent;

    FuncDtor (node);
    return AddNode ({kNum, number, parent, NULL, NULL});
}

static node_t* SimplifyAddNull (node_t* const node)
{
    ASSERT (node != NULL, "Invalid argument root = %p\n", node);

    const double zero = 0;

    node_t* result = NULL;

    if ((node->left != NULL) && (node->left->type == kNum)
        && (memcmp (&(node->left->value.number), &(zero), sizeof (zero)) == 0))
    {
        FuncDtor (node->left);
        result = node->right;
        free (node);
        return result;
    }

    if ((node->right != NULL) && (node->right->type == kNum)
        && (memcmp (&(node->right->value.number), &(zero), sizeof (zero)) == 0))
    {
        FuncDtor (node->right);
        result = node->left;
        free (node);
        return result;
    }

    return result;
}

static node_t* SimplifyFuncs (node_t* const root)
{
    ASSERT (root != NULL, "Invalid argument root = %p\n", root);

    LOG (DEBUG, "Root = %p\n", root);

    #define BINARY_FUNC(func, operation)                                                            \
        if (root->value.function == func)                                                            \
        {                                                                                           \
            return SimplifyNum (root, root->left->value.number operation root->right->value.number); \
        }

    BINARY_FUNC (kAdd, +);
    BINARY_FUNC (kSub, -);
    BINARY_FUNC (kMul, *);
    BINARY_FUNC (kDiv, /);

    #undef BINARY_FUNC

    #define UNARY_FUNC(func, operation)                                           \
        if (root->value.function == func)                                          \
        {                                                                         \
            return SimplifyNum (root, operation (root->right->value.number));     \
        }

    UNARY_FUNC (kSin, sin);
    UNARY_FUNC (kCos, cos);
    UNARY_FUNC (kTg,  tan);
    UNARY_FUNC (kCtg, ((double) 1) / tan);

    #undef UNARY_FUNC

    return NULL;
}

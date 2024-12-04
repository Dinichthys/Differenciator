#include "../differenciator.h"

#include "../../My_lib/Assert/my_assert.h"
#include "../../My_lib/Logger/logging.h"

enum DiffError DuplicateTreeDiff (node_t** const new_root, const node_t* const root)
{
    ASSERT (root     != NULL, "Invalid argument root = %p\n",     root);
    ASSERT (new_root != NULL, "Invalid argument new_root = %p\n", new_root);

    *new_root = AddNode (*root);

    if (root->left != NULL)
    {
        DuplicateTreeDiff (&((*new_root)->left), root->left);
    }

    if (root->right != NULL)
    {
        DuplicateTreeDiff (&((*new_root)->right), root->right);
    }

    return kDoneDiff;
}

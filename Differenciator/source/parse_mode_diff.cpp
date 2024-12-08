#include "differenciator.h"
#include "parse_flags_diff.h"
#include "parse_mode_diff.h"

#include "My_lib/Assert/my_assert.h"
#include "My_lib/Logger/logging.h"

enum DiffError ParseMode (const settings_of_program_t* const set, node_t** const root)
{
    ASSERT (set  != NULL, "Invalid argument set = %p\n",  set);
    ASSERT (root != NULL, "Invalid argument root = %p\n", root);

    enum DiffError result = kDoneDiff;

    switch (set->mode)
    {
        case kDiffMode:
        {
            node_t* new_root = NULL;
            result = Differencing (&new_root, *root, set->stream_out);
            *root = new_root;
            return result;
        }
        case kSolveMode:
        {

        }
        case kNoMode:
        {
            return kDoneDiff;
        }
        case kInvalidModeType:
        default:
        {
            return kInvalidModeTypeDiffError;
        }
    }
}

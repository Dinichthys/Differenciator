#ifndef STRUCT_DIFF_H
#define STRUCT_DIFF_H

#include "differenciator.h"

node_t*        ExpressionCtor (void);
enum DiffError ExpressionDtor (node_t* const root);
node_t*        AddNode        (const node_t set_val);
const char*    EnumErrorToStr (const enum DiffError error);

#endif // STRUCT_DIFF_H

#ifndef DIFF_H
#define DIFF_H

#include <stdio.h>

#include "differenciator.h"

enum DiffError Differencing (node_t** const new_root, node_t* const root, FILE* const dump_file);

#endif // DIFF_H

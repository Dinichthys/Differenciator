#ifndef READ_DIFF_H
#define READ_DIFF_H

#include <stdio.h>
#include <stdlib.h>

#include "differenciator.h"

static const size_t kLenFunc     = 100;

static const char kCommentSymbol = '#';

enum DiffError ReadDataBase (const char* const input_file_name, node_t** const root, FILE* const dump_file);

#endif // READ_DIFF_H

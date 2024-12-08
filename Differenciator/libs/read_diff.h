#ifndef READ_DIFF_H
#define READ_DIFF_H

#include <stdio.h>
#include <stdlib.h>

#include "differenciator.h"
#include "parse_flags_diff.h"

static const size_t kLenFunc       = 100;
static const size_t kExpressionLen = 200;

static const char kCommentSymbol = '#';

enum DiffError ReadExpression (settings_of_program_t* const set, node_t** const root);

#endif // READ_DIFF_H

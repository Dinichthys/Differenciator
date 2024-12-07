#ifndef DUMP_DIFF_H
#define DUMP_DIFF_H

#include <stdlib.h>

#include "differenciator.h"

static const size_t kCommandTerminalDumpLen   = 200;

static const char* const kColorNodeNum  = "green";
static const char* const kColorNodeVar  = "yellow";
static const char* const kColorNodeFunc = "brown";

enum DiffError DumpDiff (node_t* const root);

#endif // DUMP_DIFF_H

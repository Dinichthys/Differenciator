#ifndef TEX_DUMP_DIFF_H
#define TEX_DUMP_DIFF_H

#include <stdio.h>
#include <stdlib.h>

#include "differenciator.h"

static const size_t kCommandTerminalDumpTexLen = 200;
static const size_t kLenDataTex                = 100;

enum DiffError PrintFormula            (const node_t* const root, FILE* const dump_file);

enum DiffError PrintTitleTex           (FILE* const dump_file);
enum DiffError PrintEndTex             (FILE* const dump_file, const char* const file_name);

enum DiffError PrintAfterReadTreeDiff  (const char* const input_file_name, const node_t* const root,
                                        FILE* const dump_file);

enum DiffError PrintSimplify           (const node_t* const root, FILE* const dump_file);
enum DiffError PrintSimplificationEnd  (const node_t* const root, FILE* const dump_file);
enum DiffError PrintLoseSimplification (FILE* const dump_file);
enum DiffError PrintObviouslyStart     (const node_t* const root, FILE* const dump_file);
enum DiffError PrintObviouslyEnd       (const node_t* const root, FILE* const dump_file);

enum DiffError PrintDifferencing       (const node_t* const old_root, const node_t* const new_root,
                                        FILE* const dump_file);

#endif // TEX_DUMP_DIFF_H

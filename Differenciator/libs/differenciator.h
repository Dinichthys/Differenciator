#ifndef DIFFERENCIATOR_H
#define DIFFERENCIATOR_H

// REVIEW -  бля почему 1 хедер!!! пизда на диаметре

static const double kAccuracy   = 0.001;

enum DiffError
{
    kDoneDiff                   = 0,

    kCantAddNode                = 1,
    kCantCreateFunc             = 2,

    kCantReadDataBase           = 3,
    kCantCallocInputBuffer      = 4,
    kCantOpenDataBase           = 5,
    kNoBraceCloser              = 6,
    kUndefinedFuncForRead       = 7,
    kSyntaxError                = 8,

    kCantDiffTree               = 9,
    kInvalidFuncForDiff         = 10,
    kUndefinedFuncForDiff       = 11,

    kCantDumpDiff               = 12,
    kInvalidNodeType            = 13,

    kCantOpenFileDumpTexDiff    = 14,
    kCantMakePdfTexDiff         = 15,
    kInvalidNodeForPrintTexDiff = 16,
    kUndefFuncPrintTexDiff      = 17,
    kCantPrintBinaryInfixFunc   = 18,
    kCantPrintBinaryPrefixFunc  = 19,
    kCantPrintUnaryFunc         = 20,

    kInvalidModeTypeDiffError   = 21,
};

enum NodeType
{
    kNum     = 0,
    kVar     = 1,
    kFunc    = 2,
    kNewNode = 3,
};

enum FuncType
{
    kAdd = 0,
    kSub = 1,
    kMul = 2,
    kDiv = 3,
    kPow = 4,

    kSin = 5,
    kCos = 6,
    kTg  = 7,
    kCtg = 8,

    kLn  = 9,
    kLog = 10,

    kInvalidFunc = -1,
};

typedef struct node
{
    enum NodeType type;

    union value
    {
        double        number;
        char          variable;
        enum FuncType function;
    } value;

    struct node* parent;

    struct node* left;
    struct node* right;
} node_t;

#include "struct_diff.h"
#include "read_diff.h"
#include "diff.h"
#include "simplify_diff.h"
#include "dump_diff.h"
#include "tex_dump_diff.h"
#include "duplicate_tree_diff.h"
#include "connect_tree_diff.h"

#endif // DIFFERENCIATOR_H

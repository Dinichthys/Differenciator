#ifndef DIFFERENCIATOR_H
#define DIFFERENCIATOR_H

#include <stdlib.h>

static const size_t kLenNumber = 100;
static const size_t kLenFormat = 100;
static const size_t kLenFunc   = 100;
static const size_t kCommandTerminalDumpLen   = 200;

static const char* const kColorNodeNum  = "green";
static const char* const kColorNodeVar  = "yellow";
static const char* const kColorNodeFunc = "brown";


enum DiffError
{
    kDoneDiff               = 0,

    kCantAddNode            = 1,
    kCantCreateFunc         = 2,

    kCantReadDataBase       = 3,
    kCantCallocInputBuffer  = 4,
    kCantOpenDataBase       = 5,
    kNoBraceCloser          = 6,

    kCantDumpDiff           = 7,
    kInvalidNodeType        = 8,
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
    kSin = 4,
    kCos = 5,
    kTg  = 6,
    kCtg = 7,

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

node_t* FuncCtor (void);
enum DiffError FuncDtor     (node_t* const root);
enum DiffError ReadDataBase (char* const input_file_name, node_t** const root);
const char* EnumErrorToStr  (const enum DiffError error);
node_t* AddNode (const node_t set_val);
node_t* Simplify (node_t* const root);
enum DiffError DumpDiff (node_t* const root);

#endif // DIFFERENCIATOR_H

#ifndef DIFFERENCIATOR_H
#define DIFFERENCIATOR_H

#include <stdio.h>
#include <stdlib.h>

static const size_t kLenNumber  = 100;
static const size_t kLenFormat  = 100;
static const size_t kLenDataTex = 100;
static const size_t kLenFunc    = 100;
static const size_t kCommandTerminalDumpLen   = 200;

static const char kCommentSymbol = '#';

static const char* const kColorNodeNum  = "green";
static const char* const kColorNodeVar  = "yellow";
static const char* const kColorNodeFunc = "brown";


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

node_t* FuncCtor                      (void);
enum DiffError FuncDtor               (node_t* const root);
enum DiffError ReadDataBase           (const char* const input_file_name, node_t** const root, FILE* const dump_file);
enum DiffError Differencing           (node_t** const new_root, node_t* const root, FILE* const dump_file);
const char* EnumErrorToStr            (const enum DiffError error);
node_t* AddNode                       (const node_t set_val);
node_t* Simplify                      (node_t* const root, FILE* const dump_file);
enum DiffError DumpDiff               (node_t* const root);
enum DiffError PrintFormula           (const node_t* const root, FILE* const dump_file);
enum DiffError PrintTitleTex          (FILE* const dump_file);
enum DiffError PrintEndTex            (FILE* const dump_file, const char* const directory_tex,
                                                              const char* const file_name);
enum DiffError PrintAfterReadTreeDiff (const char* const input_file_name, const node_t* const root,
                                       FILE* const dump_file);
enum DiffError PrintSimplify           (const node_t* const root, FILE* const dump_file);
enum DiffError PrintSimplificationEnd  (const node_t* const root, FILE* const dump_file);
enum DiffError PrintLoseSimplification (FILE* const dump_file);
enum DiffError PrintObviouslyStart     (const node_t* const root, FILE* const dump_file);
enum DiffError PrintObviouslyEnd       (const node_t* const root, FILE* const dump_file);
enum DiffError PrintDifferencing       (const node_t* const old_root, const node_t* const new_root,
                                        FILE* const dump_file);
enum DiffError DuplicateTreeDiff       (node_t** const new_root, const node_t* const root);
enum DiffError ConnectTree             (node_t* const root);

#endif // DIFFERENCIATOR_H

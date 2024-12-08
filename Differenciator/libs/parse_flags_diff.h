#ifndef PARSE_FLAGS_DIFF_H
#define PARSE_FLAGS_DIFF_H

#include <stdio.h>

static const char* const kDiffFlag = "diff";
static const char* const kDefaultDumpTexFileName = "Dump_Files/Dump_Tex.tex";

enum ModeType
{
    kNoMode          = 0,
    kDiffMode        = 1,
    kSolveMode       = 2,

    kInvalidModeType = -1,
};

typedef struct settings_of_program
{
    FILE* stream_in;
    FILE* stream_out;
    FILE* stream_err;

    const char* input_file_name;
    const char* output_file_name;

    enum ModeType mode;

    bool stop_program;
} settings_of_program_t;

void SettingsCtor (settings_of_program_t* const set);
void SettingsDtor (settings_of_program_t* const set);
void ParseFlags   (const int argc, char* const argv[], settings_of_program_t* const set);

#endif // PARSE_FLAGS_DIFF_H

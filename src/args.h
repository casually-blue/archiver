#pragma once
#include<stdbool.h>

typedef struct {
  bool extract;

  int files_count;
  int file_actual_len;
  char** files;

  char* output_file;
} arguments;

arguments* parse_args(int argc, char** argv);

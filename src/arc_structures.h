#pragma once
#include<stdbool.h>

typedef struct {
  bool is_dir;
  char *filename;

  void* data_start;
  void* data_end;

  struct fileinfo* next_file;
} fileinfo;

typedef struct {
  unsigned long magic;
  unsigned short contents_length;

  fileinfo *first_file;
} arc_header_t;

void* memmap_file(char* filename);

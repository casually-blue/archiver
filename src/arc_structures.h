#pragma once
#include<stdbool.h>
#include<stdio.h>

typedef struct {
  bool is_nested;
  unsigned int n_entries;
} arc_header_t;

typedef struct {
  char checksum[32];
  unsigned long contents_length;
} file_info_t;

void calc_checksum(char* fname, unsigned char checksum_data[32]);
unsigned long get_length(FILE* f);

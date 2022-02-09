/**
 * @file 
 * @brief Data structures for archive data
 */

#pragma once
#include<stdbool.h>
#include<stdio.h>

typedef struct {
  bool is_nested;
  unsigned int n_entries;
} arc_header_t;

typedef struct {
  char checksum[32];
  int length;
  char* filename;
  unsigned long contents_length;
} file_info_t;

/** 
 * Calculate the sha256 checksum of a file
 *
 * @param[in] fname the input filename
 * @param[out] checksum_data the calculated checksum in binary format
 **/
void calc_checksum(char* fname, unsigned char checksum_data[32]);

unsigned long get_length(FILE* f);

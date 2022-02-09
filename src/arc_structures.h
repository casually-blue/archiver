/**
 * @file 
 * @brief Data structures for archive data
 */

#pragma once
#include<stdbool.h>
#include<stdio.h>

typedef struct {
  unsigned int n_entries;
  unsigned int namelen;
  char* name;
  struct file_info_node* first;
} arc_header_t;

typedef struct file_info_t {
  char checksum[32];
  char* filename;
  unsigned long contents_length;
} file_info_t;

typedef struct file_info_node {
  bool is_dir;

  union {
    file_info_t* file_info;
    arc_header_t* dir_info;
  } self;

  struct file_info_node* next;
} file_info_node;

/** 
 * Calculate the sha256 checksum of a file
 *
 * @param[in] fname the input filename
 * @param[out] checksum_data the calculated checksum in binary format
 **/
void calc_checksum(char* fname, unsigned char checksum_data[32]);

unsigned long get_length(FILE* f);

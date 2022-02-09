#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include <openssl/sha.h>

#include"arc_structures.h"
#include"memmap_file.h"

void calc_checksum(char* fname, unsigned char checksum_data[32]) {
  if(SHA256_DIGEST_LENGTH != 32)
    return;

  void* buffer = memmap_file(fname);

  FILE* f = fopen(fname, "r");
  int length = get_length(f);
  fclose(f);

  SHA256(buffer, length, checksum_data);

  munmap(buffer, length);
}

unsigned long get_length(FILE* f) {
  int position = ftell(f);

  fseek(f, 0, SEEK_SET);
  int start = ftell(f);

  fseek(f, 0, SEEK_END);
  int end = ftell(f);

  fseek(f, position, SEEK_SET); 

  return end - start;  
}

void write_header(FILE* of, arc_header_t* hdr) {
  fwrite(hdr, sizeof(arc_header_t), 1, of);  
}

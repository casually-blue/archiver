#include<stdlib.h>
#include<sys/mman.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/file.h>
#include<fcntl.h>
#include<errno.h>

#include"arc_structures.h"

void create_empty_file(char* fname) {
  FILE* f = fopen(fname, "a");
  fclose(f);
}

int create_sparse_file_of_n_bytes(char* fname, unsigned long int bytes) {
  create_empty_file(fname);

  int fd;
  if((fd = open(fname, O_WRONLY)) <= 0) {
    return fd;
  }

  int error = posix_fallocate(fd, bytes * sizeof(char), 1);

  if(error) return error;

  close(fd);

  return 0;
}

void* create_map_from_fd(int fd, unsigned long int bytes) {
  void* map = mmap(0, bytes * sizeof(char), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
  close(fd);

  if(!map) 
    return 0;


  return map;
}

void* memmap_file(char* filename) {
  if(create_sparse_file_of_n_bytes(filename, 8192) != 0)
    return NULL;

  int fd = open(filename, O_RDWR);

  if(fd <= 0)
    return NULL;

  void* map = create_map_from_fd(fd, 8192);

  return map;
}



#include<stdlib.h>
#include<sys/mman.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/file.h>
#include<fcntl.h>

#include"arc_structures.h"

void* memmap_file(char* filename) {
  FILE* f = fopen(filename, "a");
  fclose(f);

  int fd = open(filename, O_RDWR);
  posix_fallocate(fd, 8192 * sizeof(char), 1);

  if(fd == -1){
    return NULL;
  }

  void* map = mmap(0, 8192 * sizeof(char), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

  if(!map){
    return NULL;
  }

  close(fd);

  return map;
}



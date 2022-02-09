#include<stdlib.h>
#include<sys/mman.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/file.h>
#include<fcntl.h>
#include<errno.h>

#include"memmap_file.h"
#include "arc_structures.h"

void* create_map_from_fd(int fd, unsigned long int bytes) {
  void* map = mmap(0, bytes * sizeof(char), PROT_READ, MAP_SHARED, fd, 0);
  close(fd);

  if(!map) 
    return 0;

  return map;
}

void* memmap_file(char* filename) {
  FILE* f = fopen(filename, "r");
  unsigned long length = get_length(f);
  fclose(f);

  int fd = open(filename, O_RDONLY);

  if(fd <= 0)
    return NULL;
  
  return create_map_from_fd(fd, length);
}



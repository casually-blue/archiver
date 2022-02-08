#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/file.h>
#include<dirent.h>

#include "args.h"
#include "arc_structures.h"

void print_if_dir(char* fname) {
  int fd;
  if((fd = open(fname, O_RDONLY)) <= 0) {return;}

  struct stat fi;
  fstat(fd, &fi);

  if (S_ISDIR(fi.st_mode)) {
    printf("directory: %s\n", fname);
  }

  close(fd);
}

void truncate_to_n(char* fname, int len) {
  int fd = open(fname, O_WRONLY);

  ftruncate(fd, len);

  close(fd);
}

int main(int argc, char** argv, char** envp){
  arguments* args = parse_args(argc, argv);

  char* map = memmap_file(args->output_file);

  if(!map) {
    printf("Error, failed to map file\n");
    return -1;
  }

  int current_position = 0;

  for(int i = 0; i < args->files_count; i++){
      print_if_dir(args->files[i]);

      current_position += sprintf(map+current_position, "%s: %d\n", args->files[i], current_position);
  }

  munmap(map, 20);

  truncate_to_n(args->output_file, current_position);

  free(args->files);
  //free(args->output_file);
  free(args);

  return 0;
}

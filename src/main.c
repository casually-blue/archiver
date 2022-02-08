#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/file.h>

#include "args.h"
#include "arc_structures.h"

int main(int argc, char** argv, char** envp){
  arguments* args = parse_args(argc, argv);
  char* map = memmap_file(args->output_file);

  int current_position = 0;
  for(int i = 0; i < args->files_count; i++){
    current_position += sprintf(map+current_position, "%s: %d\n", args->files[i], current_position);
  }

  munmap(map, 20);

  int fd = open(args->output_file, O_WRONLY);
  ftruncate(fd, current_position);
  close(fd);

  free(args->files);
  free(args->output_file);
  free(args);

  return 0;
}

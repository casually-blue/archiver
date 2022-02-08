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

int main(int argc, char** argv, char** envp){
  arguments* args = parse_args(argc, argv);

  char* map = memmap_file(args->output_file);

  if(!map) {
    printf("Error, failed to map file\n");
    return -1;
  }

  int current_position = 0;

  for(int i = 0; i < args->files_count; i++){
    int fd;
    if((fd = open(args->files[i], O_RDONLY)) <= 0) {
      return -1;
    }
    struct stat fi;
    fstat(fd, &fi);

    if (S_ISDIR(fi.st_mode)) {
      printf("directory: %s\n", args->files[i]);
    }

    close(fd);
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

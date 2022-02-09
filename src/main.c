/**
 * @file
 */
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
#include "memmap_file.h"
#include<stdbool.h>

bool is_dir(char* fname) {
  int fd;
  if((fd = open(fname, O_RDONLY)) <= 0) {return false;}

  struct stat fi;
  fstat(fd, &fi);

  close(fd);

  if (S_ISDIR(fi.st_mode)) {
    return true;
  }

  return false;
}

void truncate_to_n(char* fname, int len) {
  int fd = open(fname, O_WRONLY);

  ftruncate(fd, len);

  close(fd);
}

char* concat_paths(char* p1, char* p2) {
  int dirnl = strlen(p1);
  int dnamel = strlen(p2);
  char* dn = malloc(dirnl + 1 + dnamel + 1);
  strncpy(dn, p1, dirnl);
  dn[dirnl] = '/';
  strncpy(dn+dirnl+1, p2, dnamel);
  dn[dirnl+1+dnamel] = 0;

  return dn;
}

void output_dir(char* dirname, FILE* output_buffer) {
  DIR* d;
  struct dirent *dir;
  d = opendir(dirname);

  unsigned char cksum[32];
  if(d){
    while((dir = readdir(d)) != NULL) {
      char* rel_fullpath = concat_paths(dirname, dir->d_name);
      char* fullpath = realpath(rel_fullpath, NULL);

      switch(dir->d_type){
        case DT_DIR:
          if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            break;

          fprintf(output_buffer, "Dir: %s\n", fullpath);
          output_dir(fullpath, output_buffer);
          break;
        case DT_REG:
          printf("FILE: %s\n", dir->d_name);
          calc_checksum(fullpath, cksum);

          for(int i=0; i < 32; i++){
            printf("%02x", cksum[i]);
          }
          printf("\n");

          fprintf(output_buffer, "File: %s\n", fullpath);
          
          break;
        default:
          break;
      }

      free(fullpath);
    }
  }
}

int main(int argc, char** argv, char** envp){
  arguments* args = parse_args(argc, argv);

  FILE* out_file = fopen(args->output_file, "w");

  if(!out_file) {
    printf("Error, failed to open file\n");
    return -1;
  }

  for(int i = 0; i < args->files_count; i++){
    if(is_dir(args->files[i])) {
      output_dir(args->files[i], out_file);
    } else {

      unsigned char cksum[32] = {0};
      calc_checksum(args->files[i], cksum);
      for(int i=0; i< 32; i++){
        printf("%02x", cksum[i]);
      }
      printf("\n");
      fprintf(out_file, "File: %s\n", args->files[i]);
    }
  }

  fclose(out_file);

  free(args->files);
  //free(args->output_file);
  free(args);

  return 0;
}

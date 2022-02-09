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

file_info_node* file_info(char* path){
  file_info_node* node = malloc(sizeof(file_info_node));
  unsigned char cksum[32];
  calc_checksum(path, cksum);

  file_info_t* info = malloc(sizeof(file_info_t));

  FILE* f = fopen(path, "r");
  int length = get_length(f);
  fclose(f);
          
  info->filename = path;
  info->contents_length = length;

  for(int i=0; i<32; i++){
    info->checksum[i] = cksum[i];
  }

  node->is_dir = false;
  node->self.file_info = info;

  return node;
}

void output_dir(char* dirname, arc_header_t* dir_info) {
  DIR* d;
  struct dirent *dir;
  d = opendir(dirname);

  dir_info->n_entries = 0;
  dir_info->name = dirname;
  dir_info->first = NULL;

  file_info_node* cur_node = NULL;
  file_info_node* next_node = NULL;

  if(d){
    while((dir = readdir(d)) != NULL) {

      char* rel_fullpath = concat_paths(dirname, dir->d_name);
      char* fullpath = realpath(rel_fullpath, NULL);

      switch(dir->d_type){
        case DT_DIR:
          next_node = malloc(sizeof(file_info_node));   
          if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            break;

          arc_header_t* dir_hdr = malloc(sizeof(arc_header_t));

          output_dir(fullpath, dir_hdr);

          next_node->is_dir = true;
          next_node->self.dir_info = dir_hdr;

          break;
        case DT_REG:
          next_node = file_info(fullpath);
          break;
        default:
          break;
      }

      if(dir_info->first == NULL) {
        dir_info->first = next_node;
        cur_node = next_node;
      } else {
        cur_node->next = next_node;
       cur_node = next_node;
      }

      dir_info->n_entries += 1;
    }
  }
}

int main(int argc, char** argv, char** envp){
  arguments* args = parse_args(argc, argv);

  for(int i = 0; i < args->files_count; i++){
    if(is_dir(args->files[i])) {
      arc_header_t* hdr = malloc(sizeof(arc_header_t));
      output_dir(args->files[i], hdr);

      printf("test\n");
    } else {

      unsigned char cksum[32] = {0};
      calc_checksum(args->files[i], cksum);
      for(int i=0; i< 32; i++){
        printf("%02x", cksum[i]);
      }
      printf("\n");
    }
  }

  free(args->files);
  free(args);

  return 0;
}

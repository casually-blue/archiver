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
#include<stdbool.h>

#include "args.h"
#include "arc_structures.h"
#include "memmap_file.h"
#include "varray.h"
#include "astring.h"

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

String concat_paths(char* p1, char* p2) {
  String s1 = string_from_cstring(p1);
  String s2 = string_from_cstring(p2);
  String dirsep = string_from_cstring("/");

  String s1_sep = string_concat(s1, dirsep);
  String result = string_concat(s1_sep, s2);

  string_delete(s1);
  string_delete(s2);
  string_delete(dirsep);
  string_delete(s1_sep);

  return result;
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

  dir_info->entries = varray_new(16);
  dir_info->name = dirname;

  if(d){
    while((dir = readdir(d)) != NULL) {
      file_info_node* node;

      String rel_fullpath = concat_paths(dirname, dir->d_name);
      char* fullpath = realpath(string_cstring(rel_fullpath), NULL);
      string_delete(rel_fullpath);

      switch(dir->d_type){
        case DT_DIR:
          node = malloc(sizeof(file_info_node));   
          if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            break;

          arc_header_t* dir_hdr = malloc(sizeof(arc_header_t));

          output_dir(fullpath, dir_hdr);

          node->is_dir = true;
          node->self.dir_info = dir_hdr;

          break;
        case DT_REG:
          node = file_info(fullpath);
          break;
        default:
          break;
      }

      if(node != NULL)
        varray_append(dir_info->entries, node);
    }
  }
}

int main(int argc, char** argv, char** envp){
  arguments* args = parse_args(argc, argv);

  arc_header_t* hdr;

  for(int i = 0; i < args->files_count; i++){
    if(is_dir(args->files[i])) {
      hdr = malloc(sizeof(arc_header_t));
      output_dir(args->files[i], hdr);
    }
  }

  free(args->files);
  free(args);

  return 0;
}

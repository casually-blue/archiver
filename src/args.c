#include<argp.h>
#include"args.h"
#include<stdlib.h>
#include<string.h>

const char *program_version = "arc 1.0";

static char doc[] = "File archiving utility";
static char args_doc[] = "FILES...";

static struct argp_option options[] = {
  {"extract", 'x', 0, 0, "Extract archive"},
  {"create", 'c', 0, 0, "Create archive"},
  {"file", 'f', "FILE", 0, "Output to specified file instead of {first file name}.arc"},
  {0}
};

static error_t parse_opt (int key, char* arg, struct argp_state *state) {
  arguments *arguments = state->input;

  switch(key) {
    case 'x':
      arguments->extract = true;
      break;
    case 'c':
      arguments->extract = false;
      break;
    case 'f':
      arguments->output_file = arg;
      break;

    case ARGP_KEY_ARG:
      arguments->files_count += 1;
      if(arguments->file_actual_len <= arguments->files_count) {
        arguments->file_actual_len = (arguments->file_actual_len + 1) * 2;
        arguments->files = realloc(arguments->files, sizeof(char*) * arguments->file_actual_len);
      }
      arguments->files[arguments->files_count-1] = arg;
      break;

    case ARGP_KEY_END:
      if(state->arg_num < 1)
        argp_usage(state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

arguments* parse_args(int argc, char** argv) {
  arguments* parsed_args = malloc(sizeof(arguments));

  parsed_args->extract = false;
  parsed_args->files = NULL;
  parsed_args->files_count = 0;
  parsed_args->file_actual_len = 0;
  parsed_args->output_file = NULL;

  argp_parse(&argp, argc, argv, 0, 0, parsed_args);

  if(parsed_args->output_file == 0){
    char* first_infile = parsed_args->files[0];
    int first_infile_length = strlen(first_infile);

    parsed_args->output_file = malloc(first_infile_length + strlen(".arc") + 1);

    strncpy(parsed_args->output_file, parsed_args->files[0], first_infile_length);
    strncpy(parsed_args->output_file + first_infile_length, ".arc", 5);
    parsed_args->output_file[first_infile_length + strlen(".arc")] = '\0';
  }

  return parsed_args;
}

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *read_project_dir(char *path) {
  DIR *dirp;
  int last_version = 0;
  struct dirent *entry;

  dirp = opendir(path);
  if (dirp == NULL) {
    printf("%s", "failed to open the project directory");
    exit(1);
  }

  while ((entry = readdir(dirp)) != NULL) {
    char *token = strtok(entry->d_name, "_");
    int version = atoi(token + 1);
    if (version > last_version) {
      last_version = version;
    }
  }

  last_version += 1;
  char postfix[10];
  sprintf(postfix, "%d", last_version);

  size_t totla_len = strlen(postfix) + 4;
  char *version_str = malloc(totla_len);

  if (last_version < 10) {
    strcat(version_str, "V00");
  } else if (last_version < 100 && last_version > 10) {
    strcat(version_str, "V0");
  } else {
    strcat(version_str, "V");
  }

  strcat(version_str, postfix);

  closedir(dirp);

  return version_str;
}

char *generate_file_name(char *version, char *desc) {
  time_t current_time = time(0);
  struct tm *time_info;
  char timestamp[100];

  time(&current_time);
  time_info = localtime(&current_time);
  // Get formated timestamp
  strftime(timestamp, 100, "%Y%m%d%H%M%S", time_info);
  // Allocate memory for the final string (consider strlen(another_string) +
  // strlen(timestamp) + 2 for separator and null terminator)

  size_t total_len = strlen(version) + strlen(desc) + strlen(timestamp) + 10;
  char *file_name = malloc(total_len);

  snprintf(file_name, total_len, "%s_%s__%s.%s", version, timestamp, desc,
           "sql");

  return file_name;
}

void create_file(char *filename) {

  FILE *fp = fopen(filename, "w");
  if (fp == NULL) {
    printf("Error creating file \n");
    exit(1);
  }

  fclose(fp);
}

int main(int argc, char *argv[]) {
  char *dir;
  char *desc;

  // Print help message if no arguments or "--help" is provided
  if (argc < 5 || strcmp(argv[1], "--help") == 0) {
    printf("Usage: %s [OPTIONS]\n", argv[0]);
    printf("  --dir=<directory>    Specify the directory to migrations.\n");
    printf(
        "  --desc=<description>  Provide a description for the migration.\n");
    printf("  --help           Display this help message.\n");
    return 1;
  }

  int i = 1;
  while (i < argc) {
    if (strcmp(argv[i], "--dir") == 0) {
      if (i + 1 < argc) {
        dir = argv[i + 1];
      }

      i += 2;
    } else if (strcmp(argv[i], "--desc") == 0) {
      if (i + 1 < argc) {
        desc = argv[i + 1];
      }
      i += 2;
    } else {
      printf("Uknown argument %s\n", argv[i]);
    }
  }

  char *version = read_project_dir(dir);
  char *file_name = generate_file_name(version, desc);

  // Add 1 for '/' and 1 for null terminator
  size_t total_len = strlen(dir) + strlen(file_name) + 2;
  char *file_path = malloc(total_len * sizeof(char));
  if (file_path == NULL) {
    printf("Can not create file path");
    return 1;
  }

  strcpy(file_path, dir);

  size_t dir_len = strlen(dir);

  if (dir[dir_len - 1] != '/') {
    strcat(file_path, "/");
  }

  strcat(file_path, file_name);
  create_file(file_path);
  printf("\033[32m[OK]\033[0m Migration %s created \n", file_name);
  free(file_path);
  free(file_name);
  free(version);
  return 0;
}

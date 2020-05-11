//
// Created by sachetto on 18/10/17.
//

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/mman.h>
#include <stdio.h>

#define FILE_HAS_EXTENSION(file_ext__, ext__) (strcmp(file_ext__, ext__) == 0)
#define ENDS_WITH_SLASH(path__) (path__[strlen(path__)-1] == '/')

typedef char * string_array;

struct path_information {
    bool exists, is_file, is_dir;
    char *file_extension;
    char *filename_without_extension;
    char *dir_name;
};

char * get_current_directory();
char *get_filename_ext(const char *filename);
int cp_file(const char *to, const char *from);
char * read_entire_file(const char *filename, size_t *size);
char *read_entire_file_with_mmap(const char *filename, size_t *size);
string_array list_files_from_dir(const char *dir, const char *prefix);
string_array list_files_from_dir_sorted(const char *dir, const char *prefix);
string_array read_lines(const char *filename);
bool dir_exists(const char *path);
bool file_exists(const char *path);

void get_path_information(const char *path, struct path_information *input_info );
void free_path_information(struct path_information *input_info);

char *get_filename_without_ext(const char *filename);
void fixpath(char *path);
void create_dir(char *out_dir);
int remove_directory(const char *path);
size_t base64_decode(unsigned char* out, const char *src, size_t len, size_t *bytes_read);
char * get_dir_from_path(const char * path);
char * get_file_from_path(const char * path);

#endif //FILE_UTILS_H

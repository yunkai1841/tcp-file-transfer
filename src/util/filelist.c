#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILES 1000

int filelist(char *directory, char file_list[MAX_FILES][NAME_MAX + 1]) {
    DIR *dir;
    struct dirent *entry;
    struct stat stat_buf;
    int file_count = 0;

    if ((dir = opendir(directory)) == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", directory);
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/%s", directory, entry->d_name);
        if (stat(path, &stat_buf) == 0) {
            if (S_ISDIR(stat_buf.st_mode)) {
                continue;
            }
        }
        strcpy(file_list[file_count], entry->d_name);
        file_count++;
    }

    closedir(dir);
    return file_count;
}

void print_filelist(char *directory) {
    char file_list[MAX_FILES][NAME_MAX + 1];
    int file_count = filelist(directory, file_list);
    for (int i = 0; i < file_count; i++) {
        printf("%s\n", file_list[i]);
    }
}

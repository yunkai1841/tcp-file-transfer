#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *entry;
    char *directory = argv[1];

    if ((dir = opendir(directory)) == NULL) {
        printf("Cannot open directory: %s\n", directory);
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}

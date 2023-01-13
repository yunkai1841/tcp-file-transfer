#include <limits.h>
#define MAX_FILES 1000

int filelist(char *directory, char file_list[MAX_FILES][NAME_MAX + 1]);
void print_filelist(char *directory);

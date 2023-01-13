#include <stdio.h>

#include "filelist.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }
    print_filelist(argv[1]);
    return 0;
}

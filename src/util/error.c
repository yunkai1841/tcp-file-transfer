#include <stdio.h>
#include <stdlib.h>

void exit_with_msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "FileFinder.h"

int main(int argc,char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr,"\nUsage: %s <file_name>\n\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    find_file(argv[1]);
    return 0;
}


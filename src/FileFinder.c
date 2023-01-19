#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>

#include"MyQueue.h"
#include"FileFinder.h"

unsigned int founded = 0;
const char homeDir[LEN] = "/home/vitalik13";

char *joinPath(const char *base, char *new)
{
    if (!new)
    {
        return NULL;
    }
    char *path = malloc(strlen(base) + strlen(new) + PATH_SEP_LEN);
    if (!path)
    {
        fprintf(stderr, "Cannot allocate memory");
        exit(1);
    }
    strcpy(path, base);
    if (path[strlen(path) - 1] != '/')
        strcat(path, "/");
    strcat(path, new);

    return path;
}


int findInHomeDir(threadData *info)
{
    struct dirent *dir;
    DIR *d = opendir(homeDir);
    if (d == NULL)
    {
        fprintf(stderr, "Cannot open directory-- %s", homeDir);
    }
    if (d)
    {
        while ((dir = readdir(d)) != NULL && founded == 0)
        {

            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            {
                char *path = joinPath(homeDir, dir->d_name);

                if (dir->d_type == DT_DIR)
                {
                    enqueue(path);
                }
                else
                {
                    if (strcmp(dir->d_name, info->searchedFile) == 0)
                    {
                        printf("Path to searched file: %s\n", path);
                        founded++;
                    }
                }
                free(path);
            }
        }
        closedir(d);
    }
    return 0;
}

void traverse_directory(char *name, char *currentDir)
{
    struct dirent *dir;
    DIR *d = opendir(currentDir);
    if (d == NULL)
    {
        fprintf(stderr, "Cannot open directory - %s\n", currentDir);
        return;
    }
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {

            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            {
                char *path = joinPath(currentDir, dir->d_name); 
                if(path == NULL)
                    break;

                if (dir->d_type == DT_DIR)
                { 
                    enqueue(path);
                }
                else
                {
                    if (strcmp(dir->d_name, name) == 0)
                    {
                        printf("Path to searched file: %s\n", path);
                        founded++;
                    }
                }
                free(path);
            }
        }
        closedir(d);
    }
}

void *thread_function(void *args)
{
    threadData *infoForThread = (threadData *)args;
    char *dirToFind = NULL;

    while ((dirToFind = dequeue()) != NULL)
    {
        traverse_directory(infoForThread->searchedFile, dirToFind);
        free(dirToFind);
    }
    return NULL;
}

struct dirent **getHomeDirs(const char *currentDir, threadData *dataForThreads)
{
    struct dirent **namelist;
    int n;
    n = scandir(currentDir, &namelist, NULL, alphasort);
    if (n == -1)
    {
        perror("scandir");
        exit(0);
    }
    dataForThreads->numberDir = n;
    return namelist;
}

void find_file(char* fileName)
{
    threadData *dataThreads;
    
    if(!(dataThreads = malloc(sizeof(threadData))))
    {
        fprintf(stderr,"Cannot allocate memory for threads' data");
        exit(EXIT_FAILURE);
    }

    dataThreads->searchedFile = calloc(LEN, sizeof(char));
    strcpy(dataThreads->searchedFile, fileName);
    dataThreads->homeDirs = getHomeDirs(homeDir, dataThreads);

    findInHomeDir(dataThreads);
    start_routine(dataThreads);
    free(dataThreads);
}

void start_routine(threadData *dataThreads)
{
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        if(pthread_create(&threads[i], NULL, thread_function, dataThreads) != 0)
        {
            perror("pthread_create() error");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        if(pthread_join(threads[i], NULL) != 0)
        {
            perror("pthread_join() error");
            exit(EXIT_FAILURE);
        }
    }

    if (!founded)
    {
        printf("Cannot find file with name - %s\n\n", dataThreads->searchedFile);
    }
}
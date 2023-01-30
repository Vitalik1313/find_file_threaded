#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>

#include"MyQueue.h"
#include"FileFinder.h"

pthread_t threads[THREAD_POOL_SIZE];

unsigned int founded = 0;

//Specify home directory
char homeDir[LEN] = "/home/";

char* joinPath(const char *base, char *new)
{
    if (new == NULL)
    {
        return NULL;
    }
    char *path = malloc(strlen(base) + strlen(new) + PATH_SEPARATOR_LEN + 1);
    
    if (path == NULL)
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

void traverse_directory(char *name, char *currentDir)
{
    char* path;
    struct dirent *dir;
    DIR *traversed_directory = opendir(currentDir);
    if (traversed_directory == NULL)
    {
        fprintf(stderr, "Cannot open directory - %s\n", currentDir);
        return;
    }
    
    while ((dir = readdir(traversed_directory)) != NULL)
    {

        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
        {
            path = joinPath(currentDir, dir->d_name); 
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
    closedir(traversed_directory);
    
}

void* thread_function(void *args)
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

struct dirent** getHomeDirs(const char *currentDir, threadData *dataForThreads)
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
    threadData *dataThreads = malloc(sizeof(threadData));
    
    if(dataThreads == NULL)
    {
        fprintf(stderr,"Cannot allocate memory for threads' data");
        exit(EXIT_FAILURE);
    }

    dataThreads->searchedFile = calloc(LEN, sizeof(char));
    if(dataThreads->searchedFile == NULL)
    {
        fprintf(stderr,"Cannot allocate memory for threads' data file name");
        exit(EXIT_FAILURE);
    }
    strcpy(dataThreads->searchedFile, fileName);
    dataThreads->homeDirs = getHomeDirs(homeDir, dataThreads);

    traverse_directory(dataThreads->searchedFile, homeDir);
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
#ifndef MY_FILE_FINDER
#define MY_FILE_FINDER

#define THREAD_POOL_SIZE 5
#define LEN 255
#define PATH_SEP_LEN 2

pthread_t threads[THREAD_POOL_SIZE];

extern unsigned int founded;
extern const char homeDir[LEN];

typedef struct threadInfo
{
    char *searchedFile;
    struct dirent **homeDirs;
    int numberDir;
} threadData;

int findInHomeDir(threadData* );
char *joinPath(const char *base, char *new);
void traverse_directory(char *name, char *currentDir);
void *thread_function(void *args);
struct dirent **getHomeDirs(const char *currentDir, threadData *dataForThreads);
void find_file(char* fileName);
void start_routine(threadData *dataThreads);

#endif
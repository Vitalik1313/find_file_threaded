#ifndef MY_QUEUE
#define MY_QUEUE

struct Node
{
    char* dirName;
    struct Node *next;
}; 

extern struct Node *rear; 
extern struct Node *front;
extern pthread_mutex_t mutex;

struct Node *newNode(char *dir);
char *dequeue();
void enqueue(char *dirName);

#endif
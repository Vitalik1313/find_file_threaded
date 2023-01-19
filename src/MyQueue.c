#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>

#include"MyQueue.h"

struct Node* rear = NULL;
struct Node* front = NULL;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct Node *newNode(char *dir)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    if (node != NULL)
    {
        node->dirName = malloc((strlen(dir) + 1)*sizeof(char));
        if(!(node->dirName))
        {
            fprintf(stderr,"cannot allocate memory for directory name\n");
            exit(EXIT_FAILURE);
        }
        strcpy(node->dirName,dir);
        node->next = NULL;
        return node;
    }
    else
    {
        printf("\nHeap Overflow");
        exit(EXIT_FAILURE);
    }
}

char *dequeue()
{
    pthread_mutex_lock(&mutex);

    if (front == NULL)
    {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    
    struct Node *temp = front;
    front = front->next;

    char *item = malloc((strlen(temp->dirName)+1)*sizeof(char));
    if(!item)
    {
        fprintf(stderr,"cannot allocate memory for return value\n");
        exit(EXIT_FAILURE);
    }
    strcpy(item,temp->dirName);
    free(temp);

    pthread_mutex_unlock(&mutex);
    return item;
}

void enqueue(char *dirName)
{
    struct Node *node = newNode(dirName);

    pthread_mutex_lock(&mutex);
    if (front == NULL)
    {
        front = node;
        rear = node;
    }
    else
    {
        rear->next = node;
        rear = node;
    }
    pthread_mutex_unlock(&mutex);
}
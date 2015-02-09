/**
 * exposes the interface of LinkedList.cpp
 *
 * @sourceFile LinkedList.h
 *
 * @program    Protocol Tester.exe
 *
 * @function   void linkedListInit(LinkedList*);
 * @function   void linkedListAppend(LinkedList*, void*);
 * @function   void linkedListPrepend(LinkedList*, void*);
 * @function   BOOL linkedListInsert(LinkedList*, void*, int);
 * @function   BOOL linkedListRemoveElement(LinkedList*, void*);
 * @function   BOOL linkedListRemoveByIndex(LinkedList*, int);
 * @function   int linkedListRemoveFirst(LinkedList*);
 * @function   int linkedListRemoveLast(LinkedList*);
 * @function   BOOL linkedListHas(LinkedList*, void*);
 * @function   void* linkedListAt(LinkedList*, int);
 * @function   void* linkedListFirst(LinkedList*);
 * @function   void* linkedListLast(LinkedList*);
 * @function   int linkedListSize(LinkedList*);
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric tsang
 *
 * @programmer Eric tsang
 *
 * @note       none
 */
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <windows.h>

struct Node
{
    void* data;
    struct Node* next;
    struct Node* prev;
};

struct LinkedList
{
    struct Node* head;
    struct Node* tail;
    int size;
};

typedef struct Node Node;
typedef struct LinkedList LinkedList;

void linkedListInit(LinkedList*);
void linkedListAppend(LinkedList*, void*);
void linkedListPrepend(LinkedList*, void*);
BOOL linkedListInsert(LinkedList*, void*, int);
BOOL linkedListRemoveElement(LinkedList*, void*);
BOOL linkedListRemoveByIndex(LinkedList*, int);
int linkedListRemoveFirst(LinkedList*);
int linkedListRemoveLast(LinkedList*);
BOOL linkedListHas(LinkedList*, void*);
void* linkedListAt(LinkedList*, int);
void* linkedListFirst(LinkedList*);
void* linkedListLast(LinkedList*);
int linkedListSize(LinkedList*);

#endif


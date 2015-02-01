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

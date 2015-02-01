#include "LinkedList.h"

typedef struct Node Node;

static Node* nodeMalloc(LinkedList* list, void* data, Node* prev, Node* next)
{
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->data = data;
    newNode->prev = prev;
    newNode->next = next;

    if(prev)
    {
        prev->next = newNode;
    }
    else
    {
        list->head = newNode;
    }

    if(next)
    {
        next->prev = newNode;
    }
    else
    {
        list->tail = newNode;
    }

    ++(list->size);

    return newNode;
}

static void nodeFree(LinkedList* list, Node* toDelete)
{
    if(toDelete->prev)
    {
        toDelete->prev->next = toDelete->next;
    }
    else
    {
        list->head = 0;
    }

    if(toDelete->next)
    {
        toDelete->next->prev = toDelete->prev;
    }
    else
    {
        list->head = 0;
    }

    --(list->size);

    free(toDelete);
}

void linkedListInit(LinkedList* dis)
{
    dis->head = 0;
    dis->tail = 0;
    dis->size = 0;
}

void linkedListAppend(LinkedList* dis, void* data)
{
    linkedListInsert(dis, data, linkedListSize(dis));
}

void linkedListPrepend(LinkedList* dis, void* data)
{
    linkedListInsert(dis, data, 0);
}

BOOL linkedListInsert(LinkedList* dis, void* data, int index)
{
    Node* prev = 0;
    Node* next = 0;
    BOOL inserted = FALSE;

    if(index/2 < linkedListSize(dis))
    {
        next = dis->head;
        while(index >= 0 && next != 0)
        {
            prev = next;
            next = next->next;
            --index;
        }
        if(index == 0)
        {
            nodeMalloc(dis, data, prev, next);
            inserted = TRUE;
        }
    }
    else
    {
        prev = dis->tail;
        while(index <= linkedListSize(dis) && prev != 0)
        {
            next = prev;
            prev = prev->prev;
            ++index;
        }
        if(index == linkedListSize(dis))
        {
            nodeMalloc(dis, data, prev, next);
            inserted = TRUE;
        }
    }

    return inserted;
}

BOOL linkedListRemoveElement(LinkedList* dis, void* data)
{
    Node* curr;
    BOOL removed = FALSE;

    for(curr = dis->head; curr != 0; curr = curr->next)
    {
        if(curr == data)
        {
            nodeFree(dis, curr);
            removed = TRUE;
            break;
        }
    }

    return removed;
}

BOOL linkedListRemoveByIndex(LinkedList* dis, int index)
{
    Node* curr;
    BOOL removed = FALSE;

    if(index/2 < linkedListSize(dis))
    {
        curr = dis->head;
        while(index >= 0 && curr != 0)
        {
            curr = curr->next;
            --index;
        }
        if(index == 0)
        {
            nodeFree(dis, curr);
            removed = TRUE;
        }
    }
    else
    {
        curr = dis->tail;
        while(index <= linkedListSize(dis)-1 && curr != 0)
        {
            curr = curr->prev;
            ++index;
        }
        if(index == linkedListSize(dis)-1)
        {
            nodeFree(dis, curr);
            removed = TRUE;
        }
    }

    return removed;
}

int linkedListRemoveFirst(LinkedList* dis)
{
    return linkedListRemoveByIndex(dis, 0);
}

int linkedListRemoveLast(LinkedList* dis)
{
    return linkedListRemoveByIndex(dis, linkedListSize(dis)-1);
}

BOOL linkedListHas(LinkedList* dis, void* data)
{
    Node* curr;
    BOOL has = FALSE;

    for(curr = dis->head; curr != 0; curr = curr->next)
    {
        if(curr == data)
        {
            has = TRUE;
            break;
        }
    }

    return has;
}

void* linkedListAt(LinkedList* dis, int index)
{
    Node* curr;
    void* at = NULL;

    if(index/2 < linkedListSize(dis))
    {
        curr = dis->head;
        while(index >= 0 && curr != 0)
        {
            curr = curr->next;
            --index;
        }
        if(index == 0)
        {
            at = curr->data;
        }
    }
    else
    {
        curr = dis->tail;
        while(index <= linkedListSize(dis)-1 && curr != 0)
        {
            curr = curr->prev;
            ++index;
        }
        if(index == linkedListSize(dis)-1)
        {
            at = curr->data;
        }
    }

    return at;
}

void* linkedListFirst(LinkedList* dis)
{
    return linkedListAt(dis, 0);
}

void* linkedListLast(LinkedList* dis)
{
    return linkedListAt(dis, linkedListSize(dis)-1);
}

int linkedListSize(LinkedList* dis)
{
    return dis->size;
}

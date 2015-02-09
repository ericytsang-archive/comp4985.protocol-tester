/**
 * contains functions of a classic linked list.
 *
 * @sourceFile LinkedList.cpp
 *
 * @program    ProtocolTester.exe
 *
 * @function   nodeMalloc(LinkedList* list, void* data, Node* prev, Node* next)
 * @function   static void nodeFree(LinkedList* list, Node* toDelete)
 * @function   void linkedListInit(LinkedList* dis)
 * @function   BOOL linkedListInsert(LinkedList* dis, void* data, int index)
 * @function   BOOL linkedListRemoveElement(LinkedList* dis, void* data)
 * @function   BOOL linkedListRemoveByIndex(LinkedList* dis, int index)
 * @function   BOOL linkedListHas(LinkedList* dis, void* data)
 * @function   void* linkedListAt(LinkedList* dis, int index)
 * @function   int linkedListSize(LinkedList* dis)
 * @function   void linkedListAppend(LinkedList* dis, void* data)
 * @function   void linkedListPrepend(LinkedList* dis, void* data)
 * @function   int linkedListRemoveFirst(LinkedList* dis)
 * @function   int linkedListRemoveLast(LinkedList* dis)
 * @function   void* linkedListFirst(LinkedList* dis)
 * @function   void* linkedListLast(LinkedList* dis)
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 */
#include "LinkedList.h"

typedef struct Node Node;

/**
 * creates a new node on the heap
 *
 * @function   nodeMalloc
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * creates a new node on the heap.
 *
 * @signature  nodeMalloc(LinkedList* list, void* data, Node* prev, Node* next)
 *
 * @param      list pointer to the linked list that the new node will be a part
 *   of.
 * @param      data void pointer to the data associated with this node.
 * @param      prev pointer to a node that this node's previous pointer is going
 *   to point to. the previous node's next pointer will be reassigned to the
 *   address of the new node. if this is null, then the head of the linked list
 *   will be pointed at the new node.
 * @param      next pointer to a node that this node's next pointer is going to
 *   point to. the next node's previous pointer will be reassigned to the
 *   address of the new node. if this is null, then the tail of the linked list
 *   will be pointed at the new node.
 *
 * @return     pointer to the new node.
 */
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

/**
 * removes a node from the linked list.
 *
 * @function   nodeFree
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  static void nodeFree(LinkedList* list, Node* toDelete)
 *
 * @param      list pointer to the linked list that the node is being removed
 *   from.
 * @param      toDelete pointer to the node to delete.
 */
static void nodeFree(LinkedList* list, Node* toDelete)
{
    if(toDelete->prev)
    {
        toDelete->prev->next = toDelete->next;
    }
    else
    {
        list->head = toDelete->next;
    }

    if(toDelete->next)
    {
        toDelete->next->prev = toDelete->prev;
    }
    else
    {
        list->tail = toDelete->prev;
    }

    --(list->size);

    free(toDelete);
}

/**
 * initializes an empty linked list.
 *
 * @function   linkedListInit
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  void linkedListInit(LinkedList* dis)
 *
 * @param      dis pointer to the linked list that is being operated upon.
 */
void linkedListInit(LinkedList* dis)
{
    dis->head = 0;
    dis->tail = 0;
    dis->size = 0;
}

/**
 * @see linkedListInsert
 */
void linkedListAppend(LinkedList* dis, void* data)
{
    linkedListInsert(dis, data, linkedListSize(dis));
}

/**
 * @see linkedListInsert
 */
void linkedListPrepend(LinkedList* dis, void* data)
{
    linkedListInsert(dis, data, 0);
}

/**
 * inserts a new node into the passed linked list at the specified index; the
 *   newly inserted node will have the specified index after the insertion.
 *
 * @function   linkedListInsert
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  BOOL linkedListInsert(LinkedList* dis, void* data, int index)
 *
 * @param      dis pointer to the linked list that the new data is being
 *   inserted into.
 * @param      data void pointer to the data that will be inserted into the
 *   linked list.
 * @param      index index in the linked list to insert the data into.
 *
 * @return     TRUE if the insertion was successful; FALSE otherwise.
 */
BOOL linkedListInsert(LinkedList* dis, void* data, int index)
{
    Node* prev = 0;
    Node* next = 0;
    BOOL inserted = FALSE;

    if(index/2 < linkedListSize(dis))
    {
        next = dis->head;
        while(index > 0 && next != 0)
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
        while(index < linkedListSize(dis) && prev != 0)
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

/**
 * removes a node in the linked list that's pointing at the specified data.
 *
 * @function   linkedListRemoveElement
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  BOOL linkedListRemoveElement(LinkedList* dis, void* data)
 *
 * @param      dis linked list pointer that operation is being performed on.
 * @param      data pointer to the data to be removed from the linked list.
 *
 * @return     TRUE if the element was removed; FALSE otherwise.
 */
BOOL linkedListRemoveElement(LinkedList* dis, void* data)
{
    Node* curr;
    BOOL removed = FALSE;

    for(curr = dis->head; curr != 0; curr = curr->next)
    {
        if(curr->data == data)
        {
            nodeFree(dis, curr);
            removed = TRUE;
            break;
        }
    }

    return removed;
}

/**
 * removes the node at the specified element from the linked list.
 *
 * @function   linkedListRemoveByIndex
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  BOOL linkedListRemoveByIndex(LinkedList* dis, int index)
 *
 * @param      dis linked list pointer that operation is being performed on.
 * @param      index index in the linked list to remove the node from.
 *
 * @return     TRUE if the operation succeeded, FALSE otherwise.
 */
BOOL linkedListRemoveByIndex(LinkedList* dis, int index)
{
    Node* curr;
    BOOL removed = FALSE;

    if(index/2 < linkedListSize(dis))
    {
        curr = dis->head;
        while(index > 0 && curr != 0)
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
        while(index < linkedListSize(dis)-1 && curr != 0)
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

/**
 * @see linkedListRemoveByIndex
 */
int linkedListRemoveFirst(LinkedList* dis)
{
    return linkedListRemoveByIndex(dis, 0);
}

/**
 * @see linkedListRemoveByIndex
 */
int linkedListRemoveLast(LinkedList* dis)
{
    return linkedListRemoveByIndex(dis, linkedListSize(dis)-1);
}

/**
 * returns TRUE if the linked list contains the specified data; FALSE otherwise.
 *
 * @function   linkedListHas
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  BOOL linkedListHas(LinkedList* dis, void* data)
 *
 * @param      dis linked list pointer that operation is being performed on.
 * @param      data pointer to the data to test is present in the linked list.
 *
 * @return     TRUE if the linked list contains the specified data; FALSE
 *   otherwise.
 */
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

/**
 * returns the data in the linked list at the specified index.
 *
 * @function   linkedListAt
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  void* linkedListAt(LinkedList* dis, int index)
 *
 * @param      dis linked list pointer that operation is being performed on.
 * @param      index index in the linked list to get the data from. head being
 *   index 0, and tail being at index size-1
 */
void* linkedListAt(LinkedList* dis, int index)
{
    Node* curr;
    void* at = NULL;

    if(index/2 < linkedListSize(dis))
    {
        curr = dis->head;
        while(index > 0 && curr != 0)
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
        while(index < linkedListSize(dis)-1 && curr != 0)
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

/**
 * @see linkedListAt
 */
void* linkedListFirst(LinkedList* dis)
{
    return linkedListAt(dis, 0);
}

/**
 * @see linkedListAt
 */
void* linkedListLast(LinkedList* dis)
{
    return linkedListAt(dis, linkedListSize(dis)-1);
}

/**
 * returns the number of elements in the linked list.
 *
 * @function   linkedListSize
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  int linkedListSize(LinkedList* dis)
 *
 * @param      dis linked list pointer that operation is being performed on.
 *
 * @return     number of elements in the linked list.
 */
int linkedListSize(LinkedList* dis)
{
    return dis->size;
}

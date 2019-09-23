/*
 * list.c
 *
 *  Created on: 6 Aug 2019
 *      Author: Yarden
 */


#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "types.h"

#define C_OK 0
#define C_NOK -1

static ListNode *listFindNode(List *, int);
static ListNode *listInitNode(void *);

/*
 * Initialize a new list
 * returns:   pointer to new list
 */
List *listInit()
{
  List *list = (List *) malloc(sizeof(List));
  list->size = 0;
  list->first = NULL;
  list->last = NULL;
  return list;
}

/*
 * Helper function:
 * Initialize a new node
 * in:        pointer to data
 * returns:   pointer to new node
 */
static ListNode *listInitNode(void *data)
{
  ListNode *node = (ListNode *) malloc(sizeof(ListNode));
  node->data = data;
  node->prev = NULL;
  node->next = NULL;
  return node;
}

/*
 * Get element at arbitrary position
 * in:        pointer to list
 * in:        position
 * returns:   void pointer to data / NULL on failure
 */
void *listGet(List *list, int pos)
{
  ListNode *node = listFindNode(list, pos);
  if(node != NULL)
    return node->data;
  else
    return NULL;
}

/*
 * Helper function:
 * Find node at a given position
 * in:        pointer to list
 * in:        position
 * returns:   pointer to Node / NULL on failure
 */
static ListNode *listFindNode(List *list, int pos)
{
	ListNode *currNode;
	  int currPos;
	  int reverse;

  if(pos > list->size)
    return NULL;

  /* decide where to start iterating from (font or back of the list) */
  if(pos > ((list->size-1) / 2)) {
    reverse  = 1;
    currPos  = list->size - 1;
    currNode = list->last;
  } else {
    reverse  = 0;
    currPos  = 0;
    currNode = list->first;
  }

  while(currNode != NULL) {
    if(currPos == pos)
      break;

    currNode = (reverse ? (currNode->prev) : (currNode->next));
    currPos  = (reverse ? (currPos-1) : (currPos+1));
  }
  return currNode;
}

/*
 * add element to end of list
 * in:        pointer to list
 * in:        pointer to data
 * returns:   0 on success, -1 on failure
 */
int listPushBack(List *list, void *data)
{
  /* initialize new node */
  ListNode *newNode = listInitNode(data);

  /* if list is empty */
  if(list->size == 0) {
    list->first = newNode;
  } else {
    /* if there is at least one element */
    list->last->next = newNode;
    newNode->prev = list->last;
  }
  list->last = newNode;
  list->size++;
  return C_OK;
}

/*
 * remove from an arbitrary position
 * in:        pointer to list
 * in:        pointer to data
 * returns:   0 on success, -1 on failure
 */
void *listRemove(List *list, int pos)
{
  ListNode *currNode = listFindNode(list, pos);
  void *data = NULL;

  if(currNode == NULL)
    return NULL;

  data = currNode->data;

  if(currNode->prev == NULL)
    list->first = currNode->next;
  else
    currNode->prev->next = currNode->next;

  if(currNode->next == NULL)
    list->last = currNode->prev;
  else
    currNode->next->prev = currNode->prev;

  list->size--;
  free(currNode);
  return data;
}


void *listRemoveByNode(List *list, ListNode *node)
{
  ListNode *currNode = node;
  void *data = NULL;

  if(currNode == NULL)
    return NULL;

  data = currNode->data;

  if(currNode->prev == NULL)
    list->first = currNode->next;
  else
    currNode->prev->next = currNode->next;

  if(currNode->next == NULL)
    list->last = currNode->prev;
  else
    currNode->next->prev = currNode->prev;

  list->size--;
  free(currNode);
  return data;
}

/*
 * destroys a list and frees all list related memory
 * Does not touch the data stored at the nodes
 */
void listDestroy(List *list)
{
  ListNode *currNode = list->first;
  ListNode *nextNode;

  while(currNode != NULL) {
    nextNode = currNode->next;
    free(currNode);
    currNode = nextNode;
  }
  free(list);
}

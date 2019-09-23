/*
 * list.h
 *
 *  Created on: 6 Aug 2019
 *      Author: Yarden
 */

#ifndef LIST_H_
#define LIST_H_

typedef struct node {
  void *data;
  struct node *prev;
  struct node *next;
} ListNode;

typedef struct {
  int size;
  ListNode *first;
  ListNode *last;
} List;


/*  create new list */
List *listInit();

/*  get node by position */
void *listGet(List *, int pos);

/*  add last */
int listPushBack(List *list, void *data);

/*  remove functions */
void *listRemove(List *, int);
void *listRemoveByNode(List *list, ListNode *node);
void *gllPopBack(List *);


/*  destructive functions */
void listDestroy(List *);
int removeAllMovesFromCurr(List *list, ListNode *currNode);


#endif /* LIST_H_ */

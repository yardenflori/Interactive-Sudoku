/*
 * move.c
 *
 *  Created on: 12 Aug 2019
 *      Author: Yarden
 */

#include <stdio.h>
#include <stdlib.h>
#include "move.h"
#include "types.h"
#include "game.h"
#include "list.h"

int undo(Board *board, List* list, ListNode **currentMove)
{
	Move *move;
	if(list->size==1 || list->first==*currentMove)
	{
		printf("invalid undo. first move\n");
		return 0;
	}


	move = (Move*) (*currentMove)->data;

	while(move->isFirstMoveOfCommand==0)
	{
		cancelSingleMove(board, move);
		*currentMove=(*currentMove)->prev;
		move = (Move*) (*currentMove)->data;
	}

	cancelSingleMove(board, move);
	*currentMove=(*currentMove)->prev;
	return 1;
}

int redo(Board *board, List* list, ListNode **currentMove)
{

	int flag=0;
	Move *move;
	if(list->size==1)
	{
		printf("invalid redo. no moves\n");
		return 0;
	}
	if((*currentMove)->next==NULL)
	{
		printf("invalid redo. last move\n");
		return 0;
	}

	move = (Move*) (*currentMove)->next->data;

	while(move->isLastMoveOfCommand==0)
	{
		flag=1;
		redoSingleMove(board, move);
		*currentMove=(*currentMove)->next;
		move = (Move*) (*currentMove)->data;
	}
	redoSingleMove(board, move);
	if(!flag)
		*currentMove=(*currentMove)->next;
	return 1;
}

void cancelSingleMove(Board *board, Move *move)
{
	setValueOfCell(board, move->row, move->col, move->previousValue, SOLVE);
}

void redoSingleMove(Board *board, Move *move)
{
	setValueOfCell(board, move->row, move->col, move->newValue, SOLVE);
}

void addMoveToCurrentAndCleanNextMoves (List* list, ListNode **currentMove, Move *move)
{
	removeAllMovesFromCurr(list, *currentMove);
	listPushBack(list, move);
}

void printMove(ListNode *currentMove)
{
	Move *move;
	move = (Move*) currentMove->data;
	printf("row is %d\ncol is %d\n prev is %d\nnew is %d\nis first? %d\nis last? %d\n",
			move->row, move->col, move->previousValue, move->newValue, move->isFirstMoveOfCommand, move->isLastMoveOfCommand);
	printf("----------");
}

void printMoves(List* moveList, ListNode **currentMove)
{
	int i, size;
	ListNode *node = (ListNode *) malloc(sizeof(ListNode));

	size = moveList->size;
	printf("size of list is %d\n", size);
	if (size==1)
		return;
	node = moveList->first->next;
	for(i=1; i<size; i++)
	{
		if(*currentMove==node)
			printf("this is current move:\n");
		printMove(node);
		node=node->next;
	}

	if(node != NULL) {
		free((Move *) node->data);
		free(node);
	}
}

void freeMoveList(List *list)
{
	Move *move;
  ListNode *currNode = list->first;
  ListNode *nextNode;

  while(currNode != NULL) {
    nextNode = currNode->next;
    move = (Move *) currNode->data;
    free(move);
    free(currNode);
    currNode = nextNode;
  }
}

int removeAllMovesFromCurr(List *list, ListNode *current)
{
	ListNode *currNode;
	ListNode *nextNode;

	if(list->first==NULL)
		return 1;

	currNode=current;

	if(current->next==NULL)
	{
		return 1;
	}


	currNode = currNode->next;
	nextNode = currNode->next;

	while(nextNode!=NULL)
	{
		free((Move *) currNode->data);
		listRemoveByNode(list, currNode);
		currNode=nextNode;
		nextNode=nextNode->next;
	}
	free((Move *) currNode->data);
	listRemoveByNode(list, currNode);
	return 1;
}

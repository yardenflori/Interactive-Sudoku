/*
 * move.h
 *
 *  Created on: 6 Aug 2019
 *      Author: Yarden
 */

#include "types.h"

#ifndef MOVE_H_
#define MOVE_H_

typedef struct Moves
{
	int isFirstMoveOfCommand;
	int isLastMoveOfCommand;
	int col;
	int row;
	int previousValue;
	int newValue;
}Move;

/*
 * cancel last command
 */
int undo(Board *board, List* list, ListNode **currentMove);

/*
 * redo last cancelled command
 */
int redo(Board *board, List* list, ListNode **currentMove);

/*
 * undo one move
 */
void cancelSingleMove(Board *board, Move *move);

/*
 * redo one move
 */
void redoSingleMove(Board *board, Move *move);

/*
 * add new move and clean the rest of the list
 */
void addMoveToCurrentAndCleanNextMoves (List* list, ListNode **currentMove, Move *move);

/*
 * print moves for debug
 */
void printMove(ListNode *currentMove);
void printMoves(List* moveList, ListNode **currentMove);

/*
 * free the memory for a list
 */
void freeMoveList(List *list);

/*
 * gets current and clear the list from current to end
 */
int removeAllMovesFromCurr(List *list, ListNode *current);

#endif /* MOVE_H_ */


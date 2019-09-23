/*
 * parser.h
 *
 *  Created on: 22 ���� 2019
 *      Author: yarden.flori
 */

#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_

#include <stdio.h>
#include <string.h>
#include "list.h"
#include "util.h"

/*
 * parseCommand gets from user command, and
 * determine which command it is.
 * after that, it calls to the relevant function
 * to execute the command, or print error of invalid
 * command.
 */
void parseCommand(Board **boardP, char* command, List *moveList, ListNode **curr);

/*functions for handling commands*/
void handleCommandSet(Board *board, int row, int col, int val, List *moveList,
	ListNode **curr, int isFirstMoveOfCommand, int isLastMoveOfCommand, enum boolean shouldPrint) ;
void handleCommandSolve(Board **board, char *filePath, List *moveList, ListNode **curr);
void handleCommandEdit(Board **board, char *filePath, List *moveList, ListNode **curr);
void handleCommandUndo(Board *board, List* moveList, ListNode **curr);
void handleCommandRedo(Board *board, List* moveList, ListNode **curr);
void handleCommandAutoFill(Board *board, List *moveList, ListNode **curr);
void handleCommandReset(Board *board, List *moveList, ListNode **curr);
void handleCommandGuess(Board *board, float threshold, List *moveList, ListNode **curr);
void handleCommandGenerate(int x, int y, Board *board, List *moveList, ListNode **curr);


#endif /* GAME_MANAGER_H_ */

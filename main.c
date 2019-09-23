/*
 * main.c
 *
 *  Created on: Apr 26, 2019
 *      Author: orrbo
 */
#include <stdio.h>
#include <stdlib.h>

#include "SPBufferset.h"
#include  "util.h"
#include "solver.h"
#include "board_reader.h"
#include "game.h"
#include "game_manager.h"
#include "stack.h"
#include "move.h"
#include "board_writer.h"
#include "list.h"


int main() {

	Board* board;
	List *moveList;
	ListNode *curr;
	Move *first = NULL;


	SP_BUFF_SET()
	moveList = listInit();


	listPushBack(moveList, first);
	curr=moveList->first;


	board = initEmptyBoard(9, 3, 3);

	printf("start game by entering solve command or edit command\n");
	while (1) {
		playTurn(&board, moveList, &curr);
	}

	return 0;
}

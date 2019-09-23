/*
 * parser.c
 *
 *  Created on: 22 ���� 2019
 *      Author: yarden.flori
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "board_writer.h"
#include "board_reader.h"
#include "util.h"
#include "solver.h"
#include "linear_programming_solver.h"
#include "game.h"
#include "list.h"
#include "move.h"

enum mode currentGameMode = INIT;
int markErrors = true;
List *moveList;

char *modeToName(enum mode aMode) {
	switch (aMode) {
	case INIT:
		return "Init";
	case SOLVE:
		return "Solve";
	case EDIT:
		return "Edit";
	default:
		return "Init";
	}
}

void printFormatSolveFileRequired(int isArgSpecified) {
	if (!isArgSpecified) {
		printf(
				"solve command requires one argument: file path. missing argument\n");
	} else {
		printf(
				"solve command requires only one argument: file path. too many arguments specified\n");
	}
}

void printFormatSaveFileRequired(int isArgSpecified) {
	if (!isArgSpecified) {
		printf(
				"save command requires one argument: file path. missing argument\n");
	} else {
		printf(
				"save command requires only one argument: file path. too many arguments specified\n");
	}
}

void printFormatGuessThresholdRequired(int isArgSpecified) {
	if (!isArgSpecified) {
		printf(
				"guess command requires one argument: threshold. missing argument\n");
	} else {
		printf(
				"guess command requires only one argument: threshold. too many arguments specified\n");
	}
}

void printFormatEditTooManyArg() {
	printf(
			"edit command requires only one optional argument: file path\n. too many arguments specified\n");
}

void printFormatMarkErrorsNoArg() {
	printf(
			"mark_erros command requires one argument: 0 or 1. missing argument\n");
}

void printFormatMarkErrorsTooManyArg() {
	printf(
			"mark_erros command requires only one argument: 0 or 1. too many arguments specified\n");
}

void printFormatMarkErrorsWrongArg() {
	printf(
			"mark_erros command requires one argument: 0 or 1. got argument with wrong format\n");
}

void printFormatSetTooManyArg() {
	printf(
			"set command requires only 3 arguments: col row val. too many arguments specified\n");
}

void printFormatSetTooLittleArg() {
	printf(
			"set command requires 3 arguments: col row val. missing arguments\n");
}

void printFormatRowColTooManyArg(char *command) {
	printf(
			"%s command requires only 2 arguments: col row. too many arguments specified\n",
			command);
}

void printFormatRowColLittleArg(char *command) {
	printf("%s command requires 2 arguments: col row. missing arguments\n",
			command);
}

void printFormatGenerateTooManyArg() {
	printf(
			"generate command requires only 2 arguments: X Y. too many arguments specified\n");
}

void printFormatGenerateTooLittleArg() {
	printf("generate command requires 2 arguments: X Y. missing arguments\n");
}

void printFormatCommandGetsNoArgs(char *command) {
	printf("%s command gets no arguments\n", command);
}

void printInvalidMode(char *command, enum mode *validModes, int numOfValidModes) {
	char validModesString[100], *tmpString;
	int idx;

	char *currentModeString = modeToName(currentGameMode);

	tmpString = modeToName(validModes[0]);
	strcpy(validModesString, tmpString);
	for (idx = 1; idx < numOfValidModes; idx++) {
		tmpString = modeToName(validModes[idx]);
		strcat(validModesString, ", ");
		strcat(validModesString, tmpString);
	}
	printf(
			"the command %s is only valid in %s modes. current game mode is %s\n",
			command, validModesString, currentModeString);
	fflush(stdout);
}

void printInvalidCmd() {
	printf("Error: invalid command\n");
}

void printGameOver() {
	printf("Puzzle solved successfully\n");
}

void handleCommandSolve(Board **board, char *filePath, List *moveList, ListNode **curr) {
	Board *newBoard = readBoardFromfile(filePath);

	if (newBoard == NULL) {
		printf("Error: could not load game from file: %s\n", filePath);
		return;
	}

	freeBoard(*board);
	*board = newBoard;

	if(moveList != NULL) {
		*curr=moveList->first;
		removeAllMovesFromCurr(moveList, *curr);
	}
	currentGameMode = SOLVE;

	findErrors(*board);
	printf("Loaded game from file: %s\n", filePath);

	printBoard(*board, markErrors, currentGameMode);

	fflush(stdout);

}

void handleCommandEdit(Board **board, char *filePath, List *moveList, ListNode **curr) {
	Board *newBoard;

	*curr=moveList->first;
	removeAllMovesFromCurr(moveList, *curr);
	if (filePath == NULL) {
		freeBoard(*board);
		*board = initEmptyBoard(9, 3, 3);

		currentGameMode = EDIT;
	} else {
		newBoard = readBoardFromfile(filePath);
		if (newBoard == NULL) {
			printf("Error: could not load game from file: %s\n", filePath);
			return;
		}

		freeBoard(*board);
		*board = newBoard;

		currentGameMode = EDIT;
		findErrors(*board);
	}

	printBoard(*board, markErrors, currentGameMode);

}

void handleCommandMarkErrors(int value) {
	if (value == 0) {
		markErrors = value;
		printf("mark errors is now off\n");
	} else if (value == 1) {
		markErrors = value;
		printf("mark errors is now on\n");
	} else {
		printFormatMarkErrorsWrongArg();
	}
}

void handleCommandPrintBoard(Board *board) {
	printBoard(board, markErrors, currentGameMode);
}

void handleCommandSet(Board *board, int row, int col, int val, List *moveList,
		ListNode **curr, int isFirstMoveOfCommand, int isLastMoveOfCommand, enum boolean shouldPrint) {

	int setCellResult, previousValue, index;
	enum boolean isError = false;
	Move *move;
	index = cellNum(board, row, col);
	previousValue = board->cells[index].value;

	if(row < 0 || row > board->dimension - 1 || col < 0 || col > board->dimension - 1) {
		printf("specified row or column are not in the allowed range: %d-%d\n", 1, board->dimension);
		return;
	}

	if(val < 0 || val > board->dimension) {
		printf("specified value is not in the allowed range: %d-%d\n", 1, board->dimension);
		return;
	}

	if (board->cells[cellNum(board, row, col)].isFixed && currentGameMode!=EDIT) {
		printf("Can't set value of cell (%d,%d) because it is fixed\n", row + 1,
				col + 1);
		return;
	}

	setCellResult = setValueOfCell(board, row, col, val, currentGameMode);
	findErrors(board);

	if(shouldPrint)
		printBoard(board, markErrors, currentGameMode);

	if (setCellResult == 1) {
		move = malloc(sizeof(Move));
		move->col = col, move->row = row, move->isFirstMoveOfCommand =
				isFirstMoveOfCommand, move->isLastMoveOfCommand =
						isLastMoveOfCommand, move->previousValue = previousValue, move->newValue =
								val;
		removeAllMovesFromCurr(moveList, *curr);
		listPushBack(moveList, move);
		*curr = moveList->last;
	}

	if (currentGameMode == SOLVE && board->numOfEmptyCells == 0) {
		isError = isBoardErroneous(board);
		if (isError) {
			printf("The board is full but the solution is erroneous. keep trying\n");
		} else {
			printf("Good job !! Board solved !! you can start a new game by solve or edit commands or exit\n");
			currentGameMode = INIT;
		}
	}

	return;
}

void handleCommandValidate(Board *board) {
	validate(board, true);
}

void handleCommandGuess(Board *board, float threshold, List *moveList, ListNode **curr) {
	guess(board, threshold, moveList, curr);
}

void handleCommandGenerate(int x, int y, Board *board, List *moveList, ListNode **curr) {

	int i,j, blockHeight, blockWidth, dim, indexCell, indexVal, numOfValidValues, row, col, success, isFirstMoveOfCommand, isLastMoveOfCommand;
	int *validValues=NULL;
	ListNode *prevCurr;
	LPSol *solution;
	Board *copy = (Board *) malloc(sizeof(Board));
	printf("handle generate x: %d, y: %d\n", x, y);
	prevCurr=*curr;
	copy = cpyBoard(board, copy);
	blockHeight=copy->blockHeight;
	blockWidth=copy->blockWidth;
	dim= blockHeight*blockWidth;
	success=0;
	isFirstMoveOfCommand=1;
	isLastMoveOfCommand=0;

	if(x>copy->numOfEmptyCells)
	{
		printf("There is not enough empty cells for your command\n");
		return;
	}

	if(y>dim*dim)
	{
		printf("your second parameter should be between 0 to %d\n", dim*dim);
		return;
	}

	for(j=0; j<1000; j++)
	{
		printf("iteration %d from 1000\n", j);
		for(i=0; i<x; i++)
		{
			indexCell = rand() % (dim*dim);
			while(copy->cells[indexCell].value!=0)
			{
				indexCell = rand() % (dim*dim);
			}
			row= cellRow(copy, indexCell);
			col= cellCol(copy, indexCell);
			numOfValidValues = checkValidValuesNum(copy, row, col);
			if(numOfValidValues==0)
			{
				printf("no valid values for cell %d %d in iteration %d\n", row, col, i);
				cpyBoard(board, copy);
				break;
			}
			validValues = checkValidValues(copy,row , col);
			indexVal=(rand()%numOfValidValues);
			handleCommandSet(copy, row, col, validValues[indexVal], moveList, curr, isFirstMoveOfCommand, isLastMoveOfCommand, true);
			if(isFirstMoveOfCommand==1)
			{
				isFirstMoveOfCommand=0;
			}
			printf("i is %d\n", i);
		}
		if(i!=x)
		{
			*curr=prevCurr;
			removeAllMovesFromCurr(moveList, *curr);
			isFirstMoveOfCommand=1;
			cpyBoard(board,copy);
			continue;
		}
		else
		{
			solution = LPsolve(board, true);
			if(!solution->solutionFound)
			{
				*curr=prevCurr;
				removeAllMovesFromCurr(moveList, *curr);
				isFirstMoveOfCommand=1;
				cpyBoard(board,copy);
				continue;
			}
			else
			{
				success=solve(copy, 0, moveList, curr);
				if(success)
				{
					emptyCellsFromFullBoard(copy, (dim*dim)-y, moveList, curr);
					cpyBoard(copy, board);
					((Move*) moveList->last->data)->isLastMoveOfCommand=1;
					break;
				}
				else
				{
					*curr=prevCurr;
					removeAllMovesFromCurr(moveList, *curr);
					isFirstMoveOfCommand=1;
					cpyBoard(board,copy);
					continue;
				}
			}
		}
	}

	if(solution != NULL) {
		freeLPSol(solution);
	}
	if(!success)
	{
		printf("We tried to generate for 1000 times, but unfortunately the command failed\n");
	}


}

void handleCommandUndo(Board *board, List* moveList, ListNode **curr) {
	undo(board, moveList, curr);

}

void handleCommandRedo(Board *board, List* moveList, ListNode **curr) {
	redo(board, moveList, curr);
}

void handleCommandSave(Board *board, char *filePath) {
	if (currentGameMode == EDIT && findErrors(board) == true) {
		printf(
				"Error: There are errors, please fix them before saving the board: %s\n",
				filePath);
		return;
	}

	if (currentGameMode == EDIT && !validate(board, false)) {
		printf("Error: can't save because the board is not solvable: %s\n",
				filePath);
		return;
	}

	if (writeBoardToFile(board, filePath, currentGameMode) != 0) {
		printf("Error: there was an error saving your game to file: %s\n",
				filePath);
	} else {
		printf("Your game was saved to file: %s\n", filePath);
	}
}

void handleCommandHint(Board *board, int row, int col) {
	if(row < 0 || row > board->dimension - 1 || col < 0 || col > board->dimension - 1) {
		printf("specified row or column are not in the allowed range: %d-%d\n", 1, board->dimension);
		return;
	}

	hint(board, row, col);
}

void handleCommandGuessHint(Board *board, int row, int col) {
	if(row < 0 || row > board->dimension - 1 || col < 0 || col > board->dimension - 1) {
		printf("specified row or column are not in the allowed range: %d-%d\n", 1, board->dimension);
		return;
	}

	guessHint(board, row, col);
}

void handleCommandNumSolutions(Board *board) {
	int numOfSolutions;
	numOfSolutions = findNumberOFSolutions(board);

	printf("Current board has %d solutions\n", numOfSolutions);
}

void handleCommandAutoFill(Board *board, List *moveList, ListNode **curr) {
	autoFillBoard(board, moveList, curr, true);
}

void handleCommandReset(Board *board, List *moveList, ListNode **curr) {
	while (*curr != moveList->first) {
		undo(board, moveList, curr);
	}
}

void handleCommandExit(Board *board) {
	exitGame(board);
}

void parseCommand(Board **boardP, char* command, List *moveList,
		ListNode **curr) {

	char *firstArg, *secondArg, *thirdArg, *forthArg;
	int firstIntArg, secondIntArg, thirdIntArg, isGameOverFlag;
	float threshold;
	char *token;
	Board *board = *boardP;
	enum mode validModes[3];
	/*isGameOverFlag = isGameOver(board);*/
	isGameOverFlag = 0;
	token = strtok(command, " \t\r\n");

	if (token == NULL) {
		return;
	}

	firstArg = strtok(NULL, " \t\r\n");
	secondArg = strtok(NULL, " \t\r\n");
	thirdArg = strtok(NULL, " \t\r\n");
	forthArg = strtok(NULL, " \t\r\n");

	if (isStringsEqual(token, "solve")) {

		if (firstArg == NULL) {
			printFormatSolveFileRequired(0);
			return;
		}

		if (secondArg != NULL) {
			printFormatSolveFileRequired(1);
			return;
		}

		handleCommandSolve(boardP, firstArg, moveList, curr);
		return;
	}

	else if (isStringsEqual(token, "edit")) {
		if (secondArg != NULL) {
			printFormatEditTooManyArg();
			return;
		}

		handleCommandEdit(boardP, firstArg, moveList, curr);
		return;
	}

	else if (isStringsEqual(token, "mark_errors")) {
		if (firstArg == NULL) {
			printFormatMarkErrorsNoArg();
			return;
		}

		if (secondArg != NULL) {
			printFormatMarkErrorsTooManyArg();
			return;
		}

		if (currentGameMode != SOLVE) {
			validModes[0] = SOLVE;
			printInvalidMode(token, validModes, 1);
			return;
		}

		if (!isStringsEqual(firstArg, "0") && !isStringsEqual(firstArg, "1")) {
			printFormatMarkErrorsWrongArg();
			return;
		}

		firstIntArg = atoi(firstArg);

		handleCommandMarkErrors(firstIntArg);
	}

	else if (isStringsEqual(token, "print_board")) {
		if (firstArg != NULL) {
			printFormatCommandGetsNoArgs(token);
			return;
		}

		if (currentGameMode != EDIT && currentGameMode != SOLVE) {
			validModes[0] = EDIT;
			validModes[1] = SOLVE;
			printInvalidMode(token, validModes, 2);
			return;
		}

		handleCommandPrintBoard(board);
		return;
	}

	else if (isStringsEqual(token, "set")) {
		if (firstArg == NULL || secondArg == NULL || thirdArg == NULL) {
			printFormatSetTooLittleArg();
			return;
		}

		if (forthArg != NULL) {
			printFormatSetTooManyArg();
			return;
		}

		if (currentGameMode != EDIT && currentGameMode != SOLVE) {
			validModes[0] = EDIT;
			validModes[1] = SOLVE;
			printInvalidMode(token, validModes, 2);
			return;
		}

		firstIntArg = atoi(firstArg);
		secondIntArg = atoi(secondArg);
		thirdIntArg = atoi(thirdArg);

		handleCommandSet(board, secondIntArg - 1, firstIntArg - 1, thirdIntArg,
				moveList, curr, 1, 1, true);

		return;

	}

	else if (isStringsEqual(token, "validate") && !isGameOverFlag) {
		if (firstArg != NULL) {
			printFormatCommandGetsNoArgs(token);
			return;
		}

		if (currentGameMode != EDIT && currentGameMode != SOLVE) {
			validModes[0] = EDIT;
			validModes[1] = SOLVE;
			printInvalidMode(token, validModes, 2);
			return;
		}

		handleCommandValidate(board);
	}

	else if (isStringsEqual(token, "guess")) {
		if (firstArg == NULL) {
			printFormatGuessThresholdRequired(0);
			return;
		}

		if (secondArg != NULL) {
			printFormatGuessThresholdRequired(1);
			return;
		}

		if (currentGameMode != SOLVE) {
			validModes[0] = SOLVE;
			printInvalidMode(token, validModes, 1);
			return;
		}

		threshold = atof(firstArg);

		handleCommandGuess(board, threshold, moveList, curr);
	}

	else if (isStringsEqual(token, "generate")) {
		if (firstArg == NULL || secondArg == NULL) {
			printFormatGenerateTooLittleArg();
			return;
		}

		if (thirdArg != NULL) {
			printFormatGenerateTooManyArg();
			return;
		}

		if (currentGameMode != EDIT) {
			validModes[0] = EDIT;
			printInvalidMode(token, validModes, 1);
			return;
		}

		firstIntArg = atoi(firstArg);
		secondIntArg = atoi(secondArg);

		handleCommandGenerate(firstIntArg, secondIntArg, board, moveList, curr);
	}

	else if (isStringsEqual(token, "undo")) {
		if (firstArg != NULL) {
			printFormatCommandGetsNoArgs(token);
			return;
		}

		if (currentGameMode != EDIT && currentGameMode != SOLVE) {
			validModes[0] = EDIT;
			validModes[1] = SOLVE;
			printInvalidMode(token, validModes, 2);
			return;
		}

		handleCommandUndo(board, moveList, curr);
	}

	else if (isStringsEqual(token, "redo")) {

		if (firstArg != NULL) {
			printFormatCommandGetsNoArgs(token);
			return;
		}

		if (currentGameMode != EDIT && currentGameMode != SOLVE) {
			validModes[0] = EDIT;
			validModes[1] = SOLVE;
			printInvalidMode(token, validModes, 2);
			return;
		}

		handleCommandRedo(board, moveList, curr);

	}

	else if (isStringsEqual(token, "save")) {
		if (firstArg == NULL) {
			printFormatSaveFileRequired(0);
			return;
		}

		if (secondArg != NULL) {
			printFormatSaveFileRequired(1);
			return;
		}

		if (currentGameMode != EDIT && currentGameMode != SOLVE) {
			validModes[0] = EDIT;
			validModes[1] = SOLVE;
			printInvalidMode(token, validModes, 2);
			return;
		}

		handleCommandSave(board, firstArg);
	}

	else if (isStringsEqual(token, "hint") && !isGameOverFlag) {
		if (firstArg == NULL || secondArg == NULL) {
			printFormatRowColLittleArg(token);
			return;
		}

		if (thirdArg != NULL) {
			printFormatRowColTooManyArg(token);
			return;
		}

		if (currentGameMode != SOLVE) {
			validModes[0] = SOLVE;
			printInvalidMode(token, validModes, 1);
			return;
		}

		firstIntArg = atoi(firstArg);
		secondIntArg = atoi(secondArg);

		handleCommandHint(board, secondIntArg - 1, firstIntArg - 1);
	}

	else if (isStringsEqual(token, "guess_hint") && !isGameOverFlag) {
		if (firstArg == NULL || secondArg == NULL) {
			printFormatRowColLittleArg(token);
			return;
		}

		if (thirdArg != NULL) {
			printFormatRowColTooManyArg(token);
			return;
		}

		if (currentGameMode != SOLVE) {
			validModes[0] = SOLVE;
			printInvalidMode(token, validModes, 1);
			return;
		}

		firstIntArg = atoi(firstArg);
		secondIntArg = atoi(secondArg);

		handleCommandGuessHint(board, secondIntArg - 1, firstIntArg - 1);
	}

	else if (isStringsEqual(token, "num_solutions") && !isGameOverFlag) {
		if (firstArg != NULL) {
			printFormatCommandGetsNoArgs(token);
			return;
		}

		if (currentGameMode != EDIT && currentGameMode != SOLVE) {
			validModes[0] = EDIT;
			validModes[1] = SOLVE;
			printInvalidMode(token, validModes, 2);
			return;
		}

		handleCommandNumSolutions(board);
	}

	else if (isStringsEqual(token, "autofill") && !isGameOverFlag) {
		if (firstArg != NULL) {
			printFormatCommandGetsNoArgs(token);
			return;
		}

		if (currentGameMode != SOLVE) {
			validModes[0] = SOLVE;
			printInvalidMode(token, validModes, 1);
			return;
		}

		/* handle command */

		handleCommandAutoFill(board, moveList, curr);
	}

	else if (isStringsEqual(token, "reset") && !isGameOverFlag) {
		if (firstArg != NULL) {
			printFormatCommandGetsNoArgs(token);
			return;
		}

		if (currentGameMode != EDIT && currentGameMode != SOLVE) {
			validModes[0] = EDIT;
			validModes[1] = SOLVE;
			printInvalidMode(token, validModes, 2);
			return;
		}

		handleCommandReset(board, moveList, curr);
	}

	/* there is reset instead */
	else if (isStringsEqual(token, "restart")) {
		if (firstArg != NULL) {
			printFormatCommandGetsNoArgs(token);
			return;
		}

		*boardP = restart(board);
	}

	else if (isStringsEqual(token, "exit")) {
		if (firstArg != NULL) {
			printFormatCommandGetsNoArgs(token);
			return;
		}

		if(moveList != NULL) {
			freeMoveList(moveList);
			free(moveList);
		}
		handleCommandExit(board);
		return;
	}

	else {
		printInvalidCmd();
		return;
	}

	printBoard(*boardP, markErrors, currentGameMode);
}


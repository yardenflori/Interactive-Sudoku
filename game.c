#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "board_writer.h"
#include "board_reader.h"
#include "util.h"
#include "solver.h"
#include "move.h"
#include "game_manager.h"

/*
 * game.c
 *
 *  Created on: 22 ���� 2019
 *      Author: yarden.flori
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game_manager.h"
#include "util.h"
#include "solver.h"
#include "linear_programming_solver.h"
#include "list.h"


int validateRow(Board *board, int row, int col, int value) {
	int i, testCell;
	int currentCell = cellNum(board, row, col);
	for (i = 0; i < board->dimension; i++) {
		testCell = cellNum(board, row, i);
		if (testCell != currentCell && board->cells[testCell].value == value) {
			return 0;
		}
	}
	return 1;
}

int validateCol(Board *board, int row, int col, int value) {
	int i, testCell;
	int currentCell = cellNum(board, row, col);
	for (i = 0; i < board->dimension; i++) {
		testCell = cellNum(board, i, col);
		if (testCell != currentCell && board->cells[testCell].value == value) {
			return 0;
		}
	}
	return 1;
}
int validateBlock(Board *board, int row, int col, int value) {
	int i, j, testCell;
	int blocRow = row / (board->blockHeight);
	int blocCol = col / (board->blockWidth);
	int currentCell = cellNum(board, row, col);

	for (i = blocRow * (board->blockHeight);
			i < blocRow * (board->blockHeight) + board->blockHeight; i++) {
		for (j = blocCol * (board->blockWidth);
				j < blocCol * (board->blockWidth) + board->blockWidth; j++) {
			testCell = cellNum(board, i, j);
			if (testCell != currentCell
					&& board->cells[testCell].value == value) {
				return 0;
			}
		}
	}
	return 1;
}


int validateValue(Board *board, int row, int col, int value) {
	int valid;

	if(value == 0) {
		return true;
	}
	valid = validateBlock(board, row, col, value)
										&& validateRow(board, row, col, value)
										&& validateCol(board, row, col, value);

	return valid;
}


int findErrors(Board *board) {
	int i;
	enum boolean result = false;
	for(i = 0; i < (board->dimension)*(board->dimension); i++) {
		if(!validateValue(board, cellRow(board, i), cellCol(board, i), board->cells[i].value)) {
			board->cells[i].isError = true;
			result = true;
		} else {
			board->cells[i].isError = false;
		}

	}

	return result;
}

int setValueOfCell(Board *board, int row, int col, int value, enum mode currentGameMode) {
	int oldValue;
	if (board->cells[cellNum(board, row, col)].isFixed && currentGameMode!=EDIT) {
		printf("Error: cell is fixed\n");
		return 0;
	}

	oldValue = board->cells[cellNum(board, row, col)].value;
	board->cells[cellNum(board, row, col)].value = value;
	board->cells[cellNum(board, row, col)].isFixed = false;

	if(oldValue != 0 && value == 0)
		board->numOfEmptyCells++;
	else if(oldValue == 0 && value != 0)
		board->numOfEmptyCells--;

	if (validateValue(board, row, col, value)) {
		board->cells[cellNum(board, row, col)].isError = false;
	} else {
		board->cells[cellNum(board, row, col)].isError = true;
	}

	findErrors(board);

	return 1;
}

int clearCell(Board *board, int row, int col) {
	if (board->cells[cellNum(board, row, col)].isFixed) {
		printf("Error: cell is fixed\n");
		return 0;
	}

	board->cells[cellNum(board, row, col)].value = 0;
	board->cells[cellNum(board, row, col)].isError = false;
	board->numOfEmptyCells++;

	return 1;
}

void hint(Board *board, int row, int col) {
	int v, index;
	double value;
	LPSol *solution;

	if(isBoardErroneous(board)) {
		printf("Error: cannot show hint because the board is erroneous\n");
		return;
	} else if(board->cells[cellNum(board, row, col)].isFixed) {
		printf("Error: cannot show hint because the cell (%d,%d) is fixed\n", row+1, col+1);
		return;
	} else if(board->cells[cellNum(board, row, col)].value != 0) {
		printf("Error: cannot show hint because the cell (%d,%d) contains value\n", row+1, col+1);
		return;
	}

	solution = LPsolve(board, true);
	if(!solution->solutionFound) {
		printf("Error: can't show hint because board is unsolvable\n");
		freeLPSol(solution);
		return;
	}

	for (v = 1; v <= board->dimension; v++) {
		index = getVarIndex(solution, row, col, v);
		if(index == -1)
			continue;
		value = solution->theSolution[index];
		if(value == 1.0) {
			printf("Set value of (%d, %d) to %d\n", row+1, col+1, v);
			break;
		}
	}

	freeLPSol(solution);
}

void guess(Board *board, float threshold, List *moveList, ListNode **curr) {
	int v, index, i, row, col, chosenValue = -1;
	int isFirstMoveOfCommand, isLastMoveOfCommand;
	float maxLPValue = -1;
	double value;
	LPSol *solution;

	isFirstMoveOfCommand=1;
	isLastMoveOfCommand=0;

	if(isBoardErroneous(board)) {
		printf("Error: cannot guess because the board is erroneous\n");
		return;
	}


	solution = LPsolve(board, false);
	if(!solution->solutionFound) {
		printf("Error: can't guess because board is unsolvable\n");
		freeLPSol(solution);
		return;
	}

	for(i = 0; i < (board->dimension)*(board->dimension); i++) {
		row = cellRow(board, i);
		col = cellCol(board, i);

		if(board->cells[i].value != 0) {
			continue;
		}

		chosenValue = -1;
		maxLPValue = -1;
		for (v = 1; v <= board->dimension; v++) {
			if(!validateValue(board, row, col ,v)) {
				continue;
			}
			index = getVarIndex(solution, row, col, v);
			if(index == -1)
				continue;
			value = solution->theSolution[index];
			if(value >= threshold && value > maxLPValue) {
				chosenValue = v;
				maxLPValue = value;
			}
		}

		if(chosenValue > 0) {
			handleCommandSet(board, row, col, chosenValue, moveList, curr, isFirstMoveOfCommand, isLastMoveOfCommand, false);
			if (isFirstMoveOfCommand==1)
				{
					isFirstMoveOfCommand=0;
				}
		}
	}
	if(moveList->size > 1)
	{
		(((Move*) moveList->last->data)->isLastMoveOfCommand)=1;
	}

	freeLPSol(solution);
}


void guessHint(Board *board, int row, int col) {
	int v, index;
	double value;
	LPSol *solution;

	if(isBoardErroneous(board)) {
		printf("Error: cannot show hint because the board is erroneous\n");
		return;
	} else if(board->cells[cellNum(board, row, col)].isFixed) {
		printf("Error: cannot show hint because the cell (%d,%d) is fixed\n", row+1, col+1);
		return;
	} else if(board->cells[cellNum(board, row, col)].value != 0) {
		printf("Error: cannot show hint because the cell (%d,%d) contains value\n", row+1, col+1);
		return;
	}

	solution = LPsolve(board, false);
	if(!solution->solutionFound) {
		printf("Error: can't show guess hint because board is unsolvable\n");
		freeLPSol(solution);
		return;
	}

	printf("guess hint for cell (%d,%d) is:\n", row+1, col+1);
	for (v = 1; v <= board->dimension; v++) {
		index = getVarIndex(solution, row, col, v);
		if(index == -1)
			continue;
		value = solution->theSolution[index];
		if(value > 0) {
			printf("%d with probability of %f\n", v, value);
		}
	}
	freeLPSol(solution);

}
enum boolean validate(Board *board, enum boolean shouldPrint) {
	LPSol *solution;
	if(isBoardErroneous(board)) {
		printf("Error: cannot validate because the board is erroneous\n");
		return false;
	}

	solution = LPsolve(board, true);
	if(solution->solutionFound) {
		if(shouldPrint)
			printf("Validation passed: board is solvable\n");
		freeLPSol(solution);
		return true;
	} else {
		if(shouldPrint)
			printf("Validation failed: board is unsolvable\n");
		freeLPSol(solution);
		return false;
	}

	freeLPSol(solution);

}

void playTurn(Board **boardP, List *moveList, ListNode **curr) {
	char *result;
	char cmd[MAX_CHARS_IN_COMMAND];
	result = getStringFromUser(cmd);
	if (!result) {
		printf("Exiting...\n");
		freeBoard(*boardP);
		exit(1);
	}
	parseCommand(boardP, cmd, moveList, curr);
}

void exitGame(Board *board) {
	freeBoard(board);
	printf("Exiting...\n");
	exit(0);
}

Board *initEmptyBoard(int dimension, int blockHeight, int blockWidth) {
	int i = 0;
	Cell *cells;
	Board *board = (Board *) malloc(sizeof(Board));

	cells = (Cell *) malloc((dimension * dimension) * sizeof(Cell));

	for (i = 0; i < dimension * dimension; i++) {
		cells[i].value = 0;
		cells[i].isFixed = 0;
		cells[i].isError = 0;
	}

	i = 0;

	board->dimension = dimension;
	board->cells = cells;
	board->numOfEmptyCells = dimension * dimension;
	board->blockHeight = blockHeight;
	board->blockWidth = blockWidth;

	return board;
}

Board* initGameWithNumberOfCellsToFill(int dimension, int blockHeight,
		int blockWidth, int numberOfCellsToFill) {
	int i = 0, randCol, randRow;
	Cell *cells1, *cells2;
	Board *solution = (Board *) malloc(sizeof(Board));
	Board *board = (Board *) malloc(sizeof(Board));

	cells1 = (Cell *) malloc((dimension * dimension) * sizeof(Cell));
	cells2 = (Cell *) malloc((dimension * dimension) * sizeof(Cell));

	for (i = 0; i < dimension * dimension; i++) {
		cells1[i].value = 0;
		cells1[i].isFixed = 0;
		cells1[i].isError = 0;
		cells2[i].value = 0;
		cells2[i].isFixed = 0;
		cells2[i].isError = 0;
	}

	i = 0;

	board->dimension = dimension;
	board->cells = cells1;
	board->numOfEmptyCells = dimension * dimension;
	board->blockHeight = blockHeight;
	board->blockWidth = blockWidth;

	solution = cpyBoard(board, solution);

	randomizeBackTracking(solution, solution);

	while (i < numberOfCellsToFill) {
		randCol = rand() % dimension;
		randRow = rand() % dimension;
		if (!board->cells[cellNum(board, randRow, randCol)].isFixed) {
			board->cells[cellNum(board, randRow, randCol)] =
					solution->cells[cellNum(board, randRow, randCol)];
			board->cells[cellNum(board, randRow, randCol)].isFixed = 1;
			board->numOfEmptyCells--;
			i++;
		}
	}

	free(solution->cells);
	free(solution);

	printBoard(board, 1, EDIT);

	return board;
}

Board* initGame(int test) {
	int inputBlockHeight=-10, inputBlockWidth = -10, inputNumberOfCellsToFill=-10;
	int numberOfCellsToFill = -1,blockHeight=-1, blockWidth=-1;
	int dimension;

	printf("Please enter the block height:\n");
	if(test==1)
	{
		blockHeight=3;
		inputBlockHeight=1;
		printf("block height is %d:\n", blockHeight);
	}
	else
		inputBlockHeight = scanf("%d", &blockHeight);



	while((inputBlockHeight!=1 && inputBlockHeight!=EOF)
			|| (blockHeight<1 && inputBlockHeight!=EOF))
	{
		printf("Please enter valid block height:\n");
		while ((getchar()) != '\n');
		inputBlockHeight = scanf("%d", &blockHeight);
	}


	if(test==0)
		while ((getchar()) != '\n');

	printf("Please enter the block width:\n");

	if(test==1)
	{

		blockWidth=4;
		inputBlockWidth=1;
		printf("block width is %d:\n", inputBlockWidth);
	}
	else
		inputBlockWidth = scanf("%d", &blockWidth);

	while((inputBlockWidth!=1 && inputBlockWidth!=EOF)
			|| (blockWidth<1 && inputBlockWidth!=EOF))
	{
		printf("Please enter valid block width:\n");
		while ((getchar()) != '\n');
		inputBlockWidth = scanf("%d", &blockWidth);
	}

	dimension=blockHeight*blockWidth;

	if(test==0)
		while ((getchar()) != '\n');

	printf("Please enter the number of cells to fill [0-%d]:\n", dimension*dimension-1);
	if(test==1)
	{
		numberOfCellsToFill=0;
		inputNumberOfCellsToFill=1;
		printf("number of cells is %d:\n", numberOfCellsToFill);
	}
	else
		inputNumberOfCellsToFill = scanf("%d", &numberOfCellsToFill);


	if (inputNumberOfCellsToFill == EOF || inputBlockHeight==EOF || inputBlockWidth == EOF) {
		printf("Exiting...\n");
		exit(0);
	}

	while ((numberOfCellsToFill < 0 || numberOfCellsToFill > dimension*dimension - 1) && inputNumberOfCellsToFill != EOF) {
		printf(
				"Error: invalid number of cells to fill (should be between 0 and %d)\n",dimension*dimension-1);
		printf("Please enter the number of cells to fill [0-%d]:\n", dimension*dimension-1);
		while ((getchar()) != '\n');
		inputNumberOfCellsToFill = scanf("%d", &numberOfCellsToFill);
	}

	if (inputNumberOfCellsToFill == EOF) {
		printf("Exiting...\n");
		exit(0);
	}

	return initGameWithNumberOfCellsToFill(dimension, blockHeight, blockWidth,
			numberOfCellsToFill);

}

int findNumberOFSolutions(Board *board) {
	return exhaustiveBackTracking(board);
}

void autoFillBoard(Board *board, List *moveList, ListNode **curr, enum boolean doPrint) {
	int i, *validValue = NULL, row, col;
	int isFirstMoveOfCommand, isLastMoveOfCommand;
	Board *tmp = (Board *) malloc(sizeof(Board));
	enum boolean didChange = false;
	if(isBoardErroneous(board))
	{
		printf("error in autofill command. board is erroneous\n");
		free(tmp);
		return;
	}

	isFirstMoveOfCommand=1;
	isLastMoveOfCommand=0;
	cpyBoardAsFixed(board, tmp);
	for(i = 0; i < (board-> dimension) * (board-> dimension); i++) {
		row = cellRow(board, i);
		col = cellCol(board, i);
		if(checkValidValuesNum(tmp,row, col) == 1 && !isCellFixed(tmp, row, col)) {
			didChange = true;
			validValue = checkValidValues(tmp,row , col);
			handleCommandSet(board, row, col, validValue[0], moveList, curr, isFirstMoveOfCommand, isLastMoveOfCommand, false);
			if (isFirstMoveOfCommand==1)
			{
				isFirstMoveOfCommand=0;
			}
			if(doPrint) {
				printf("Auto filled cell (%d,%d) to %d\n", col+1, row+1, validValue[0]);
			}

			if(validValue != NULL)
				free(validValue);
		}
	}

	if(moveList->size > 1)
		(((Move*) moveList->last->data)->isLastMoveOfCommand)=1;

	if(doPrint && !didChange) {
		printf("nothing to autofill\n");
	}

	freeBoard(tmp);
}

Board* restart(Board *board) {
	freeBoard(board);
	return initGame(0);
}

int isGameOver(Board *board) {
	return board->numOfEmptyCells == 0;
}

void emptyCellsFromFullBoard(Board *board, int numOfCells, List *moveList, ListNode **curr)
{
	int i, index, dim, row, col;
	dim = board->blockHeight*board->blockWidth;
	i=0;

	while(i<numOfCells)
	{
		index = rand() % (dim*dim);
		while (board->cells[index].value==0)
		{
			index = rand() % (dim*dim);
		}
		row= cellRow(board, index);
		col= cellCol(board, index);
		handleCommandSet(board, row, col, 0, moveList, curr, 0, 0, false);
		i++;
	}
}

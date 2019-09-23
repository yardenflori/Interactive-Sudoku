/*
 * solver.c
 *
 *  Created on: 22 ���� 2019
 *      Author: yarden.flori
 */
#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "game.h"
#include "game_manager.h"
#include "stack.h"
#include "linear_programming_solver.h"

int solve(Board *board, int print, List *moveList, ListNode **curr) {
	int row, col, val, index;
	LPSol *solution;

	solution = LPsolve(board, true);

	if( !solution->solutionFound )
	{
		if(print == 1)
		{
			printf("Failed to find solution to the given board\n");
		}
		return 0;
	}


	for (val = 1; val <= board->dimension; val++) {
		for (row = 0; row < board->dimension; row++) {
			for (col = 0; col < board->dimension; col++) {

				index = getVarIndex(solution, row, col, val);
				if(index != -1 && (solution->theSolution)[index] == 1.0) {
					handleCommandSet(board, row, col, val, moveList, curr, 0, 0, false);
					break;
				}
			}
		}
	}

	freeLPSol(solution);
	return 1;
}



int handleCell(Board *board, int row, int col, int value,
		int (*callback)(Board *, int, int)) {
	int result;
	setValueOfCell(board, row, col, value, SOLVE);
	if (!isLastCell(board, row, col)) {
		if (isLastCellInRow(board, col)) {
			result = callback(board, row + 1, 0);
		} else
			result = callback(board, row, col + 1);
	}
	return result;

}

int recursiveBackTrackingStep(Board *board, int row, int col) {
	int idx, result;

	if (isCellFixed(board, row, col)) {
		if (!validateValue(board, row, col,
				board->cells[cellNum(board, row, col)].value))
			return 0;
		if (isLastCell(board, row, col)) {
			return 1;
		}
		if (isLastCellInRow(board, col))
			return recursiveBackTrackingStep(board, row + 1, 0);

		return recursiveBackTrackingStep(board, row, col + 1);
	}

	for (idx = 1; idx <= board->dimension; idx++) {
		if (validateValue(board, row, col, idx)) {
			result = handleCell(board, row, col, idx,
					recursiveBackTrackingStep);
			if (result)
				return 1;
		}
	}
	clearCell(board, row, col);
	return 0;
}

int recursiveBackTracking(Board *board, Board *destination) {
	int isSuccess;
	destination = cpyBoard(board, destination);
	isSuccess = recursiveBackTrackingStep(destination, 0, 0);

	return isSuccess;
}

StackCell *nextNonFixedCell(Board *board, int cellNum) {
	int foundFirstCell = 0;
	int numOfCells = (board->dimension)*(board->dimension);
	StackCell *cell = (StackCell *) malloc(sizeof(StackCell));

	while(!foundFirstCell && cellNum !=  numOfCells) {
		if(!isCellFixed(board, cellRow(board, cellNum), cellCol(board, cellNum))){
			foundFirstCell = 1;
		} else {
			cellNum += 1;
		}
	}

	if(!foundFirstCell) {
		return NULL;
	}

	cell->cellNum = cellNum;
	cell->value = 0;
	cell->isFixed = isCellFixed(board, cellRow(board, cellNum), cellCol(board, cellNum));

	return cell;
}

int exhaustiveBackTracking(Board *board) {
	int counter = 0, newVal;
	Board *cpy = (Board *) malloc(sizeof(Board));
	StackCell *cell, *newCell;
	Stack *stack = createStack();

	cpy = cpyBoardAsFixed(board, cpy);
	cell = nextNonFixedCell(cpy, 0);
	push(stack, cell);

	while(!isEmpty(stack)) {
		cell = (StackCell *) pop(stack);
		newVal = (cell-> value) + 1;

		if(newVal > cpy->dimension) {
			clearCell(cpy, cellRow(cpy, cell->cellNum), cellCol(cpy, cell->cellNum));
			free(cell);
			continue;
		}

		if(validateValue(cpy, cellRow(cpy, cell->cellNum), cellCol(cpy, cell->cellNum), newVal)) {
			cell->value = newVal;
			setValueOfCell(cpy, cellRow(cpy, cell->cellNum), cellCol(cpy, cell->cellNum), newVal, SOLVE);
			newCell = nextNonFixedCell(cpy, (cell->cellNum)+1);
			if(newCell == NULL) {
				clearCell(cpy, cellRow(cpy, cell->cellNum), cellCol(cpy, cell->cellNum));
				counter++;
			} else {
				push(stack, cell);
				push(stack, newCell);
			}
		}
		else {
			cell->value = newVal;
			push(stack, cell);
		}
	}

	free(cpy);
	return counter;
}

int checkValidValuesNum(Board *board, int row, int col) {
	int idx, numOfValidValues = 0;
	for (idx = 1; idx <= board->dimension; idx++) {
		if (validateValue(board, row, col, idx)) {
			numOfValidValues++;
		}
	}
	return numOfValidValues;
}

int *checkValidValues(Board *board, int row, int col) {
	int idx, counter, numOfValidValues = 0, *validValues;
	int *flags = calloc(board->dimension, sizeof(int));
	for (idx = 1; idx <= board->dimension; idx++) {
		if (validateValue(board, row, col, idx)) {
			flags[idx - 1] = 1;
			numOfValidValues++;
		}
	}

	validValues = (int *) malloc(numOfValidValues * sizeof(int));

	counter = 0;
	for (idx = 0; idx < board->dimension; idx++) {
		if (flags[idx] == 1)
			validValues[counter++] = idx + 1;
	}

	free(flags);

	return validValues;

}

int randomizeBackTrackingStep(Board *board, int row, int col) {
	int idx, result = 0, currentValueIdx, currentIdx, currentValue = 0;
	int *validValues = NULL, numOfValidValues = 0, *validIndexes = NULL;

	numOfValidValues = checkValidValuesNum(board, row, col);
	validValues = checkValidValues(board, row, col);


	if (numOfValidValues == 1) {
		currentValue = validValues[0];
		result = handleCell(board, row, col, currentValue,
				randomizeBackTrackingStep);
		if (result)
			return 1;

	} else if (numOfValidValues > 1) {
		validIndexes = createRange(numOfValidValues, validIndexes);

		for (idx = 0; idx < numOfValidValues; idx++) {
			if ((numOfValidValues - idx) == 1) {
				currentValue = validValues[validIndexes[0]];
				result = handleCell(board, row, col, currentValue,
						randomizeBackTrackingStep);
				if (result)
					return 1;

				break;
			}

			currentIdx = rand() % (numOfValidValues - idx);
			currentValueIdx = validIndexes[currentIdx];
			currentValue = validValues[currentValueIdx];
			validIndexes = removeAtIndex(validIndexes, (numOfValidValues - idx),
					currentIdx);
			result = handleCell(board, row, col, currentValue,
					randomizeBackTrackingStep);
			if (result)
				return 1;

		}
	}

	clearCell(board, row, col);
	return 0;
}

int randomizeBackTracking(Board* board, Board* destination) {
	int isSuccess;
	destination = cpyBoard(board, destination);
	isSuccess = randomizeBackTrackingStep(destination, 0, 0);

	return isSuccess;
}


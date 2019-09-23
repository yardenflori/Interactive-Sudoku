/*
 * util.h
 *
 *  Created on: May 23, 2019
 *      Author: orrbo
 */
#include "types.h"

#ifndef UTIL_H_
#define UTIL_H_

/*
 * printing a board
 */
void printBoard(Board* board, enum boolean markErros, enum mode currentMode);

/*
 * returns a pointer to array in length numOfValidValues,
 * where array[i]=i;
 */
int *createRange(int numOfValidValues, int *validIndexes);

/**
 * Remove element in index idx from the given array
 */
int *removeAtIndex(int *array, int size, int idx);

/*
 * gets board, row and col.
 * returns the position in the one dimension
 * array of row,col.
 */
int cellNum(Board* board, int row, int col);

/*
 * returns the row of a cell in
 * the one dimension array of board
 */
int cellRow(Board* board, int cellNum);

/*
 * returns the col of a cell in
 * the one dimension array of board
 */
int cellCol(Board* board, int cellNum);

/*
 * copy the content of *board to *destination
 */
Board* cpyBoard(Board* board, Board* destination);

/*
 * Copies board, and each value that exists on the board is copied as fixed vlaue
 */
Board *cpyBoardAsFixed(Board* board, Board* destination);

/*
 * copy the content of *src and returns
 * a pointer to this content
 */
Cell *cpyCellArray(Cell *src, int size);


/*
 * checks whether or not cell fixed.
 */
int isCellFixed(Board* board, int row, int col);

/*
 * checks if board[row][col] is the last cell in the board
 */
int isLastCell(Board* board, int row, int col);

/**
 * Checks if the given column is the last column
 */
int isLastCellInRow(Board* board, int col);

/*
 * Checks if given board contains errors
 */
int isBoardErroneous(Board *board);

/*
 * free the memoery of *board
 */
void freeBoard(Board *board);

/*
 * checks whether or not string1 equal to string2
 */
int isStringsEqual(char *string1, char *string2);

/*
 * read input from user
 */
char *getStringFromUser(char *cmd);

void convertArrToZeros(int *arr, int length);

/*
 * initiates linear programming solution object
 */
LPSol *initLPSol(int dimension);

/*
 * free linear programming solution object
 */
void freeLPSol(LPSol *solution);

#endif /* UTIL_H_ */

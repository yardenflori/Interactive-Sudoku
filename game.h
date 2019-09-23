/*
 * game.h
 *
 *  Created on: 22 ���� 2019
 *      Author: yarden.flori
 */

#ifndef GAME_H_
#define GAME_H_

#include "list.h"
#include "util.h"
/*
 * validateValue gets a board, row, col, and value.
 * it checks whether or not value might be set to
 * the cell board[row][col]. i.e. there are no
 * conflicts with same row/col/block.
 */
int validateValue(Board *board, int row, int col, int value);

/*
 * setValueOfCell gets a board, row, col and value.
 * It check if setting board[row][col] is possible.
 * If yes, it is do it. Else, it prints the relevant
 * error.
 */
int setValueOfCell(Board *board, int row, int col, int value, enum mode currentGameMode);

/*
 * clearCell clears the cell board[row][col].
 * if cell is fixed, it's print relevant error
 * and skips the command.
 */
int clearCell(Board *board, int row, int col);

/*
 * initGame asks the user for numberOfCellsToFill input
 * and return a pointer to a board with the desired parameters
 */
Board* initGame(int test);

/*
 * initiates game with specified parameters
 */
Board* initGameWithNumberOfCellsToFill(int dimension, int blockHeight,
		int blockWidth, int numberOfCellsToFill);

Board *initEmptyBoard(int dimension, int blockHeight, int blockWidth);

/*
 * playTurn asks the user for command input,
 * and call for the relevant function to
 * execute it.
 */
void playTurn(Board **boardP, List *moveList, ListNode **curr);

/*
gets a board and indexes, and prints a possible value for the relevant cell
*/
void hint(Board *board, int row, int col);

/**
 * Solves the board using LP, and fills all cells with legal values with a score greater then x
 */
void guess(Board *board, float threshold, List *moveList, ListNode **curr);


/*
 * prints all legal values and their LP score
 */
void guessHint(Board *board, int row, int col);

/*
 * returns whether or not the board has a valit solution
 */
enum boolean validate(Board *board, enum boolean shouldPrint);

/*
 * Finds the number of existing solution to the given board
 */
int findNumberOFSolutions(Board *board);

/*
 * Fills cells with only one legal value.
 * If doPrint is not 0, then prints every change
 */
void autoFillBoard(Board *board, List *moveList, ListNode **curr, enum boolean doPrint);

/*
 * restart free the memory used for the current board
 * and initialize new board.
 */
Board* restart(Board *board);

/*
 * free the memory and exit the game.
 */
void exitGame(Board *board);

/*
 * checks for errors in the board
 */
int findErrors(Board *board);

/*
 * checks whether or not game completed.
 */
int isGameOver(Board *board);

/*
 * clear numOfCells cells from the board
 */
void emptyCellsFromFullBoard(Board *board, int numOfCells, List *moveList, ListNode **curr);

#endif /* GAME_H_ */

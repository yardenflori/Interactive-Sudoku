/*
 * solver.h
 *
 *  Created on: 22 ���� 2019
 *      Author: yarden.flori
 */

#ifndef SOLVER_H_
#define SOLVER_H_

/**
 * Solve the given board using the back tracking algorithm.
 * The solution would be stored at destination at the end of the run if
 * the board is solvable.
 * Return 1 if solution was found or 0 otherwise.
 */
int recursiveBackTracking(Board* board, Board* destination);

/**
 * Solve the given board using the random back tracking algorithm.
 * The solution would be stored at destination at the end of the run if
 * the board is solvable.
 * Return 1 if solution was found or 0 otherwise.
 */
int randomizeBackTracking(Board* board, Board* destination);


int exhaustiveBackTracking(Board *board);

/*
 * returns the number of valid values in the given cell
 */
int checkValidValuesNum(Board *board, int row, int col);

/*
 * returns array of valid values in the given cell
 */
int *checkValidValues(Board *board, int row, int col);

/*
 * solves the board if possible
 */
int solve(Board *board, int print, List *moveList, ListNode **curr);

#endif /* SOLVER_H_ */

/*
 * linear_programming_solver.h
 *
 *  Created on: Aug 20, 2019
 *      Author: orrbo
 */

#ifndef LINEAR_PROGRAMMING_SOLVER_H_
#define LINEAR_PROGRAMMING_SOLVER_H_

#include "types.h"

/*
 * solve a board using LP
 */
LPSol *LPsolve(Board *board, enum boolean isInteger);

/*
 * gets row and col, and returns the relevant index in the solution
 */
int getVarIndex(LPSol *solution, int row, int col, int value);

#endif /* LINEAR_PROGRAMMING_SOLVER_H_ */

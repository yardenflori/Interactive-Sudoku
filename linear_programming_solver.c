/*
 * linear_programming_solver.c
 *
 *  Created on: Aug 20, 2019
 *      Author: orrbo
 */

#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "game.h"
#include "gurobi_c.h"

/*
 * free the Gurobi model and env, then free memory resources
 */
void destroy(GRBenv *env, GRBmodel *model, int *ind, double *val,
		double *obj, char *vtype) {
	/* freeing Gurobi models */
	GRBfreemodel(model);
	GRBfreeenv(env);

	/* free used resources */
	if (ind != NULL) {
		free(ind);
	}
	if (val != NULL) {
		free(val);
	}
	if (obj != NULL) {
		free(obj);
	}
	if (vtype != NULL) {
		free(vtype);
	}
}

void addVar(LPSol *solution, int i, int j, int v, int index) {
	intTuple2 *tuple = malloc(sizeof(intTuple2));
	int dimension = solution->dimension;
	if (solution->varIndexes[i * dimension + j] == NULL) {
		solution->varIndexes[i * dimension + j] = listInit();
	}

	tuple->item1 = v;
	tuple->item2 = index;
	listPushBack(solution->varIndexes[i * dimension + j], tuple);
}

int addVariables(Board *board, LPSol *solution) {
	int i, val, counter = 0, row, col;
	enum boolean valueFound;
	for(i = 0; i < (board->dimension)*(board->dimension); i++) {
		row = cellRow(board, i);
		col = cellCol(board, i);
		if(board->cells[i].value != 0) {
			continue;
		}

		valueFound = false;
		for(val = 1; val <= board->dimension; val++) {
			if(validateValue(board, row, col, val)) {
				valueFound = true;
				addVar(solution, row, col, val, counter);
				counter++;
			}
		}

		if (!valueFound) { /* An empty cell without legal values */
			printf("cell (%d, %d) has no legal values", row, col);
			addVar(solution, row, col, -1, -1);
		}
	}

	return counter;
}

enum boolean isCellHasValue(LPSol *solution, int i, int j) {
	if(solution->varIndexes[i*(solution->dimension) + j] == NULL) {
		return true;
	}

	return false;
}

int getVarIndex(LPSol *solution, int row, int col, int value) {

	int size, idx;
	intTuple2 *tuple;
	List *list = solution->varIndexes[row*(solution->dimension) + col];
	if(list == NULL) {
		return -1;
	}

	size = list->size;

	for(idx = 0; idx < size; idx++) {
		tuple = (intTuple2 *) listGet(list, idx);
		if(tuple->item1 == value) {
			return tuple->item2;
		}
	}

	return -1;
}

int addCellConstraints(Board *board, LPSol *solution, int *ind, double *val, GRBmodel *model, GRBenv *env, double *obj, char *vtype) {
	int i, v, constraints = 0, varNum = 0, row, col, error;

	for(i = 0; i < (board->dimension)*(board->dimension); i++) {
		row = cellRow(board, i);
		col = cellCol(board, i);

		if(isCellHasValue(solution, row, col)) {
			continue;
		}

		varNum = 0;
		for (v = 1; v <= solution->dimension; v++) {
			if (getVarIndex(solution, row, col, v) == -1) { /* value v is not available for this cell */
				continue;
			}
			ind[varNum] = getVarIndex(solution, row, col, v);
			val[varNum] = 1.0;
			varNum++;
		}

		if (varNum == 0) {
			printf("cell (%d, %d) has no legal values", row, col);
			destroy(env, model, ind, val, obj, vtype);
			solution->solutionFound = false;
			return -1;
		}

		constraints++;
		error = GRBaddconstr(model, varNum, ind, val, GRB_EQUAL, 1.0, NULL);
		if (error) {
			printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
			destroy(env, model, ind, val, obj, vtype);
			return -1;
		}

	}

	return constraints;
}

int addRowConstraints(Board *board, LPSol *solution, int *ind, double *val, GRBmodel *model, GRBenv *env, double *obj, char *vtype) {
	int v, row, col, index, varNum, constraints = 0, error;
	enum boolean valueFound = false;

	for (v = 1; v <= board->dimension; v++) {
		for (row = 0; row < (board->dimension); row++) {
			varNum = 0;

			for (col = 0; col < board->dimension; col++) {
				if (board->cells->value == v) {
					/* the value v was found in the row so we don't set the constraint */
					valueFound = true;
					break;
				}

				index = getVarIndex(solution, row, col, v);
				if (index != -1) {

					ind[varNum] = index;
					val[varNum] = 1.0;
					varNum++;
				}

			}

			if (varNum != 0 && !valueFound) {
				/* There are constraints for the i row and the value v, and the value v was not found on that row */
				constraints++;
				error = GRBaddconstr(model, varNum, ind, val, GRB_EQUAL, 1.0, NULL);
				if (error) {
					printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					destroy(env, model, ind, val, obj, vtype);
					return -1;
				}
			}

			valueFound = false;
		}
	}

	return constraints;
}


enum boolean addColConstraints(Board *board, LPSol *solution, int *ind, double *val, GRBmodel *model, GRBenv *env, double *obj, char *vtype) {
	int v, row, col, index, varNum, constraints = 0, error = 0;
	enum boolean valueFound = false;

	for (v = 1; v <= board->dimension; v++) {
		for (col = 0; col < (board->dimension); col++) {

			varNum = 0;
			for (row = 0; row < board->dimension; row++) {
				if (board->cells->value == v) {
					/* the value v was found in the column so we don't set the constraint */
					valueFound = true;
					break;
				}

				index = getVarIndex(solution, row, col, v);
				if (index != -1) {

					ind[varNum] = index;
					val[varNum] = 1.0;
					varNum++;
				}

			}

			if (varNum != 0 && !valueFound) {
				/* There are constraints for the i column and the value v, and the value v was not found on that column */
				constraints++;
				error = GRBaddconstr(model, varNum, ind, val, GRB_EQUAL, 1.0, NULL);
				if (error) {
					printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					destroy(env, model, ind, val, obj, vtype);
					return -1;
				}
			}

			valueFound = false;
		}
	}

	return constraints;
}

int addBlockConstraints(Board *board, LPSol *solution, int *ind, double *val, GRBmodel *model, GRBenv *env, double *obj, char *vtype) {
	int i, j, v, row, col, index, varNum = 0, constraints = 0, error;
	enum boolean valueFound = false;
	/* iterate over the blocks */
	for (i = 0; i < board->blockWidth; i++) {
		for (j = 0; j < board->blockHeight; j++) {
			for (v = 1; v <= board->dimension; v++) {
				varNum = 0;

				/* iterate over the block cells */
				for (row = i * board->blockHeight; row < (i + 1) * board->blockHeight; row++) {
					for (col = j * board->blockWidth; col < (j + 1) * board->blockWidth; col++) {
						if (board->cells->value == v) {
							/* the value v was found in the block so we don't set the constraint */
							valueFound = true;
							break;
						}

						index = getVarIndex(solution, row, col, v);
						if (index != -1) {

							ind[varNum] = index;
							val[varNum] = 1.0;
							varNum++;
						}
					}
				}

				if (varNum != 0 && !valueFound) {
					/* There are constraints for the i,j block and the value v, and the value v was not found on that block */
					constraints++;
					error = GRBaddconstr(model, varNum, ind, val, GRB_EQUAL, 1.0, NULL);
					if (error) {
						printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
						destroy(env, model, ind, val, obj, vtype);
						return -1;
					}
				}

				valueFound = false;
			}
		}
	}

	return constraints;
}


LPSol *LPsolve(Board *board, enum boolean isInteger) {
	GRBenv *env = NULL;
	GRBmodel *model = NULL;
	int error = 0, result = 0;
	double *sol = NULL;
	int *ind = malloc(board->dimension * sizeof(int));
	double *val = malloc(board->dimension * sizeof(double));
	double *obj = NULL;
	char *vtype = NULL;
	int optimstatus;
	double objval;

	int i, numVars = 0;
	LPSol *solution = initLPSol(board->dimension);


	/* Create environment - log file is interactive_sudoku.log */
	error = GRBloadenv(&env, "interactive_sudoku.log");
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		return solution;
	}

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		return solution;
	}

	/* Create an empty model named "interactive_sudoku" */
	error = GRBnewmodel(env, &model, "interactive_sudoku", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		return solution;
	}

	/* Add variables  */

	numVars = addVariables(board, solution);


	vtype = malloc(numVars * sizeof(char));
	obj = malloc(numVars * sizeof(double));

	sol = malloc(numVars * sizeof(double));
	solution->theSolution = sol;

	if (isInteger) {
		for (i = 0; i < numVars; i++) {
			obj[i] = 0;
			vtype[i] = GRB_BINARY;
		}
	} else {
		for (i = 0; i < numVars; i++) {
			obj[i] = 1 + rand() % 10;
			vtype[i] = GRB_CONTINUOUS;
		}
	}

	/* add variables to model */
	error = GRBaddvars(model, numVars, 0, NULL, NULL, NULL, obj, NULL, NULL,
			vtype, NULL);
	if (error) {
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		return solution;
	}

	/* Change objective sense to maximization */
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		return solution;
	}

	/* update the model - to integrate new variables */
	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		return solution;
	}


	result = addCellConstraints(board, solution, ind, val, model, env, obj, vtype);
	if(result == -1) {
		return solution;
	}

	result = addRowConstraints(board, solution, ind, val, model, env, obj, vtype);
	if(result == -1) {
		return solution;
	}

	result = addColConstraints(board, solution, ind, val, model, env, obj, vtype);
	if(result == -1) {
		return solution;
	}

	result = addBlockConstraints(board, solution, ind, val, model, env, obj, vtype);
	if(result == -1) {
		return solution;
	}

	/* Optimize model - need to call this before calculation */
	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		return solution;
	}

	/* Write model to 'interactive_sudoku.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "interactive_sudoku.lp");
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		return solution;
	}

	/* Get solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		return solution;
	}

	/* get the objective -- the optimal result of the function */
	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error) {
		printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		solution->solutionFound = false;
		return solution;
	}

	/* get the solution - the assignment to each variable */
	/* numVars -- number of variables, the size of "sol" should match */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, numVars, sol);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error,
				GRBgeterrormsg(env));
		destroy(env, model, ind, val, obj, vtype);
		return solution;
	}

	/* print results */
	/*printf("\nOptimization complete\n");*/

	/* solution found */
	if (optimstatus == GRB_OPTIMAL) {
		solution->solutionFound = true;
	}
	/* no solution found */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		printf("Model is infeasible or unbounded\n");
		solution->solutionFound = false;
	}
	/* error or calculation stopped */
	else {
		printf("Optimization was stopped early\n");
	}

	destroy(env, model, ind, val, obj, vtype);

	return solution;
}

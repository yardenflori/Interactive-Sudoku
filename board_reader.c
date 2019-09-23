/*
 * board_reader.c
 *
 *  Created on: Aug 8, 2019
 *      Author: orrbo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

Board *readBoardFromfile(char * filePath) {
	char line[1024], *part;
	FILE *fptr;
	Board *board;
	int blockHeight, blockWidth, idx, val, length, dimension, numOfEmptyCells =
			0;
	int foundHeight = 0, foundWidth = 0;
	Cell *cells;
	if ((fptr = fopen(filePath, "r")) == NULL) {
		printf("Error: Could not open the given file: %s\n", filePath);
		return NULL;
	}

	idx = 0;
	while (fgets(line, sizeof line, fptr) != NULL) {
		part = strtok(line, " \t\r\n");
		while (part != NULL) {
			if (!foundHeight) {
				foundHeight = 1;
				blockHeight = atoi(part);
				part = strtok(NULL, " \t\r\n");
				continue;

			}
			if (!foundWidth) {
				foundWidth = 1;
				blockWidth = atoi(part);
				part = strtok(NULL, " \t\r\n");
				dimension = blockHeight * blockWidth;
				cells = (Cell *) calloc(dimension * dimension, sizeof(Cell));
				continue;

			}
			length = strlen(part);

			cells[idx].isFixed = 0;
			if (part[length - 1] == '.') {
				part[length - 1] = '\0';
				if (atoi(part) == 0) {
					printf(
							"Error: encountered non legal fixed empty cell  while reading file: %s\n",
							filePath);

					free(cells);
					fclose(fptr);
					return NULL;
				}
				cells[idx].isFixed = 1;
			}

			val = atoi(part);
			if (val == 0)
				numOfEmptyCells++;
			if (val < 0 || val > (blockHeight * blockWidth)) {
				printf(
						"Error: encountered invalid cell with value %d while reading file: %s\n",
						val, filePath);

				free(cells);
				fclose(fptr);
				return NULL;
			}

			cells[idx].value = val;
			cells[idx].isError = 0;

			idx++;
			part = strtok(NULL, " \t\r\n");
		}
	}

	if (idx != dimension * dimension) {
		printf(
				"Error: invalid number of cells. expected %d cells but found %d cells instead, while value while reading file: %s\n",
				dimension * dimension, idx, filePath);
		return NULL;
		free(cells);
	}

	board = (Board *) malloc(sizeof(Board));
	board->blockHeight = blockHeight;
	board->blockWidth = blockWidth;
	board->dimension = blockHeight * blockWidth;
	board->cells = cells;
	board->numOfEmptyCells = numOfEmptyCells;

	fclose(fptr);
	return board;
}

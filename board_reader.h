/*
 * Board_reader.h
 *
 *  Created on: Aug 8, 2019
 *      Author: orrbo
 */

#ifndef BOARD_READER_H_
#define BOARD_READER_H_

#include "types.h"

/*gets file path with data which represent board, and return Board struct with this data*/
Board *readBoardFromfile(char * filePath);

#endif /* BOARD_READER_H_ */



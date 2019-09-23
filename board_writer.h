/*
 * board_writer.h
 *
 *  Created on: Aug 8, 2019
 *      Author: orrbo
 */

#ifndef BOARD_WRITER_H_
#define BOARD_WRITER_H_

#include "types.h"

/*gets Board struct and file path, and writes the board data to the file*/
int writeBoardToFile(Board *board, char *filePath, enum mode gameMode);

#endif /* BOARD_WRITER_H_ */



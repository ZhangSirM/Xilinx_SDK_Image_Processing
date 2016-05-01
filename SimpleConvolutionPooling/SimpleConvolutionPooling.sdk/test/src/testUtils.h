/*
 * testUtils.h
 *
 *  Created on: 2016/4/25
 *      Author: anson
 */

#ifndef TESTUTILS_H_
#define TESTUTILS_H_

#include <stdio.h>
#include "sizeWeightDefinitions.h"
#include "xil_types.h"
#include "utils.h"

float processWindowAndKernel(float window[KERNEL_DIM][KERNEL_DIM]);
void poolingByHand(float imageIn[INPUT_IMG_CHANNELS * IMG_HEIGHT_OR_ROWS * IMG_WIDTH_OR_COLS],
		float imageOut[OUTPUT_IMG_CHANNELS * OUT_IMG_HEIGHT_OR_ROWS * OUT_IMG_WIDTH_OR_COLS]);

#endif /* TESTUTILS_H_ */

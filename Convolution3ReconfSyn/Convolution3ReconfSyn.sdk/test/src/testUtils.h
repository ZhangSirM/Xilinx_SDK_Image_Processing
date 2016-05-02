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

float processWindowAndKernel(int outputChannel, float *kernel, float *bias, char *window, bool relu);
void conv2dByHand(float imageIn[INPUT_IMG_CHANNELS * IMG_HEIGHT_OR_ROWS * IMG_WIDTH_OR_COLS],
		float imageOut[OUTPUT_IMG_CHANNELS * OUT_IMG_HEIGHT_OR_ROWS * OUT_IMG_WIDTH_OR_COLS],
		float kernel[OUTPUT_IMG_CHANNELS * INPUT_IMG_CHANNELS * KERNEL_DIM * KERNEL_DIM],
		float bias[OUTPUT_IMG_CHANNELS],
		int kSize, bool isImage, bool relu);
void saveTxt(const char *path,
		float *imageOut);
void saveTxtHW(const char *path,
		u32 *imageOut);
int subtractMean(int currPixVal, int channel);



#endif /* TESTUTILS_H_ */

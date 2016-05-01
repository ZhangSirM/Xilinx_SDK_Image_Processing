#include "testUtils.h"

// Do pooling by finding largest value in window
float processWindowAndKernel(float window[KERNEL_DIM][KERNEL_DIM])		{
	float currMax = 0;

	for (int idxRow = 0; idxRow < KERNEL_DIM; idxRow++)
		for (int idxCol = 0; idxCol < KERNEL_DIM; idxCol++)
			if ((float)window[idxRow][idxCol] > currMax)
				currMax = (float)window[idxRow][idxCol];

	return currMax;
}

void poolingByHand(float imageIn[INPUT_IMG_CHANNELS * IMG_HEIGHT_OR_ROWS * IMG_WIDTH_OR_COLS],
		float imageOut[OUTPUT_IMG_CHANNELS * OUT_IMG_HEIGHT_OR_ROWS * OUT_IMG_WIDTH_OR_COLS])		{

	// Sampling window (Should be the same size of the kernel)
	float samplingWindow[INPUT_IMG_CHANNELS][KERNEL_DIM][KERNEL_DIM];

	// Iterate on the image pixels
	for (int r = 0; r < OUT_IMG_HEIGHT_OR_ROWS; r++)		{
		for (int c = 0; c < OUT_IMG_WIDTH_OR_COLS; c++)	{
			// Populate our window by sampling on the image (Load input map)
			for (int rowsWindow = 0; rowsWindow < KERNEL_DIM; rowsWindow++)	{
				for (int colsWindow = 0; colsWindow < KERNEL_DIM; colsWindow++)	{
					int repos_row = r * STRIDE + rowsWindow;
					int repos_col = c * STRIDE + colsWindow;
					for (int n = 0; n < INPUT_IMG_CHANNELS; n++)	{
						float currPixVal;
						if (repos_row < IMG_HEIGHT_OR_ROWS && repos_col < IMG_WIDTH_OR_COLS)
							currPixVal = imageIn[n * IMG_HEIGHT_OR_ROWS * IMG_WIDTH_OR_COLS
													  + repos_row * IMG_WIDTH_OR_COLS + repos_col];
						else
							currPixVal = 0;

						samplingWindow[n][rowsWindow][colsWindow] = currPixVal;
					}
				}
			}
			for (int m = 0; m < OUTPUT_IMG_CHANNELS; m++)	{
				// Now multiply the window by the kernel and sum it's result
				imageOut[m * OUT_IMG_HEIGHT_OR_ROWS * OUT_IMG_WIDTH_OR_COLS
						 + r * OUT_IMG_WIDTH_OR_COLS + c]
						 = processWindowAndKernel(samplingWindow[m]);
			}
		}
	}
}


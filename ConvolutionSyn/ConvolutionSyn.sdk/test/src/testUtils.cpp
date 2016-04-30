#include "testUtils.h"

void saveTxt(const char *path,
		float *imageOut)	{
	FILE *pFile;
	pFile = fopen(path, "w");
	if (pFile != NULL)		{
		for (int m = 0; m < OUTPUT_IMG_CHANNELS; m++)	{
			for (int r=0; r < OUT_IMG_HEIGHT_OR_ROWS; r++)	{
				for (int c=0; c < OUT_IMG_WIDTH_OR_COLS; c++)	{
						fprintf (pFile, "%f ", imageOut[m * OUT_IMG_HEIGHT_OR_ROWS * OUT_IMG_WIDTH_OR_COLS
														+ r * OUT_IMG_WIDTH_OR_COLS + c]);
				}
				fprintf (pFile, "\n");
			}
		}
	}
}

void saveTxtHW(const char *path,
		u32 *imageOut)	{
	FILE *pFile;
	pFile = fopen(path, "w");
	if (pFile != NULL)		{
		for (int r=0; r < OUT_IMG_HEIGHT_OR_ROWS; r++)	{
			for (int c=0; c < OUT_IMG_WIDTH_OR_COLS; c++)	{
				for (int m = 0; m < OUTPUT_IMG_CHANNELS; m++)	{
						fprintf (pFile, "%f ", fixed2float(imageOut[r * OUT_IMG_WIDTH_OR_COLS * OUTPUT_IMG_CHANNELS
														+ c * OUTPUT_IMG_CHANNELS + m], 32, 16));
				}
				fprintf (pFile, "\n");
			}
		}
	}
}

// The convolution is all about summing the product of the window and the kernel
float processWindowAndKernel(int outputChannel, float *kernel, float *bias,
		float (*window)[KERNEL_DIM][KERNEL_DIM], bool relu)		{
	float accumulator = bias[outputChannel];

	for (int idxRow = 0; idxRow < KERNEL_DIM; idxRow++)
		for (int idxCol = 0; idxCol < KERNEL_DIM; idxCol++)
			for (int n = 0; n < INPUT_IMG_CHANNELS; n++)	{
				accumulator += (kernel[outputChannel * INPUT_IMG_CHANNELS * KERNEL_DIM * KERNEL_DIM
									   + n * KERNEL_DIM * KERNEL_DIM
									   + idxRow * KERNEL_DIM + idxCol]
										* (float)window[n][idxRow][idxCol]);
			}

	if (relu && accumulator < 0)
		return 0;
	return accumulator;
}

// Subtract mean from pixel if input is image (first layer)
int subtractMean(int currPixVal, int channel)	{
	if (channel == 0)
		currPixVal -= 104;
	else if (channel == 1)
		currPixVal -= 117;
	else
		currPixVal -= 123;
	return currPixVal;
}

void conv2dByHand(float imageIn[INPUT_IMG_CHANNELS * IMG_HEIGHT_OR_ROWS * IMG_WIDTH_OR_COLS],
		float imageOut[OUTPUT_IMG_CHANNELS * OUT_IMG_HEIGHT_OR_ROWS * OUT_IMG_WIDTH_OR_COLS],
		float kernel[OUTPUT_IMG_CHANNELS * INPUT_IMG_CHANNELS * KERNEL_DIM * KERNEL_DIM],
		float bias[OUTPUT_IMG_CHANNELS],
		int kSize, bool isImage, bool relu)		{

	int numKernelElements = kSize * kSize * INPUT_IMG_CHANNELS;

	// Sampling window (Should be the same size of the kernel)
	float samplingWindow[INPUT_IMG_CHANNELS][KERNEL_DIM][KERNEL_DIM];

	// Iterate on the image pixels
	for (int r = 0; r < OUT_IMG_HEIGHT_OR_ROWS; r++)		{
		for (int c = 0; c < OUT_IMG_WIDTH_OR_COLS; c++)	{
			// printf("Bias value: %f\n", bias[0]);
			// Populate our window by sampling on the image (Load input map)
			for (int rowsWindow = 0; rowsWindow < kSize; rowsWindow++)	{
				for (int colsWindow = 0; colsWindow < kSize; colsWindow++)	{
					int repos_row = r + rowsWindow;
					int repos_col = c + colsWindow;
					for (int n = 0; n < INPUT_IMG_CHANNELS; n++)	{
						float currPixVal = imageIn[n * IMG_HEIGHT_OR_ROWS * IMG_WIDTH_OR_COLS
												  + repos_row * IMG_WIDTH_OR_COLS + repos_col];
						// Subtract mean from pixel if input is image (first layer)
						if (isImage)
							currPixVal = subtractMean(currPixVal, n);
						samplingWindow[n][rowsWindow][colsWindow] = currPixVal;
					}
				}
			}
			for (int m = 0; m < OUTPUT_IMG_CHANNELS; m++)	{
				// Now multiply the window by the kernel and sum it's result
				imageOut[m * OUT_IMG_HEIGHT_OR_ROWS * OUT_IMG_WIDTH_OR_COLS
						 + r * OUT_IMG_WIDTH_OR_COLS + c]
						 = processWindowAndKernel(m, (float*)kernel, (float *)bias,
						samplingWindow, relu);
			}
		}
	}

}


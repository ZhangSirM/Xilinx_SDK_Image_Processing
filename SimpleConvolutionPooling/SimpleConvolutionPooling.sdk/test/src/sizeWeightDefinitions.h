#ifndef SIZEWEIGHTDEFINITIONS_H_
#define SIZEWEIGHTDEFINITIONS_H_

#define OUTPUT_IMAGE_REF_TXT "/home/anson/ZynqProjects/images/SimpleConvolutionPooling/FilterResultARM.txt"
#define OUTPUT_IMAGE_TXT "/home/anson/ZynqProjects/images/SimpleConvolutionPooling/FilterResultAcc.txt"

// 3x3 kernel
#define KERNEL_DIM 3

#define STRIDE 2
#define INPUT_IMG_CHANNELS 16
#define IMG_WIDTH_OR_COLS 110
#define IMG_HEIGHT_OR_ROWS 80
#define OUTPUT_IMG_CHANNELS 16
#define OUT_IMG_WIDTH_OR_COLS ((IMG_WIDTH_OR_COLS - KERNEL_DIM) / 2 + 2)
#define OUT_IMG_HEIGHT_OR_ROWS ((IMG_HEIGHT_OR_ROWS - KERNEL_DIM) / 2 + 2)

#define INPUT_SIZE (IMG_HEIGHT_OR_ROWS * IMG_WIDTH_OR_COLS * INPUT_IMG_CHANNELS)
#define OUTPUT_SIZE (OUT_IMG_HEIGHT_OR_ROWS * OUT_IMG_WIDTH_OR_COLS * OUTPUT_IMG_CHANNELS)



#endif /* SIZEWEIGHTDEFINITIONS_H_ */

/*
 * Commands to download memory to file (Download 76800 bytes from address 0x1300000 to file logresp.txt
 set logfile [open "/home/anson/ZynqProjects/images/FullyConvoultionalLayer/HWoutput.txt" "w"]
 puts $logfile [mrd 0x1300000 140800]
 close $logfile
 *
 * */

#include <stdio.h>
#include "xaxidma.h"
#include "platform.h"
#include "xdoconvolutionsyn.h"
#include "AxiTimerHelper.h"

#include "sizeWeightDefinitions.h"
#include "utils.h"
#include "testUtils.h"

 // Load image converted to header file
#include "/home/anson/ZynqProjects/images/FaceDetection/HeaderFiles/img_352_resized.h"
#include "/home/anson/ZynqProjects/images/FaceDetection/HeaderFiles/img_352_resized_hw.h"
 // Load weights and bias
#include "/home/anson/ZynqProjects/images/FaceDetection/Weights/face12c_full_conv/conv1_w.h"
#include "/home/anson/ZynqProjects/images/FaceDetection/Weights/face12c_full_conv/conv1_b.h"


// Memory used by DMA
#define MEM_BASE_ADDR   0x01000000
#define TX_BUFFER_BASE  (MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE  (MEM_BASE_ADDR + 0x00300000)
// Get a pointer to the TX and RX dma buffer (CONFIGURE DMA)
// The pointers are for 8-bit memory but their addresses are 32 bit (u32)
u32 *m_dma_buffer_TX = (u32*) TX_BUFFER_BASE;
u32 *m_dma_buffer_RX = (u32*) RX_BUFFER_BASE;

XAxiDma axiDmaImg;
XAxiDma axiDmaWeight;
XAxiDma axiDmaBias;

XDoconvolutionsyn doConvolutionSyn;

u32 imgIn_HW[INPUT_SIZE];
u32 conv1_w_HW[WEIGHT_SIZE];
u32 conv1_b_HW[BIAS_SIZE];
u32 res_hw[OUTPUT_SIZE];
float res_sw[OUTPUT_SIZE];

int main()		{
	int status;
	init_platform();

	AxiTimerHelper axiTimer;

	printf("Doing convolution on SW\n");
	axiTimer.startTimer();
	conv2dByHand(img_352_resized, res_sw, conv1_w, conv1_b, KERNEL_DIM, true, false);
	axiTimer.stopTimer();
	double convolution_SW_elapsed = axiTimer.getElapsedTimerInSeconds();
	printf ("Convolution SW execution time: %f sec \n", convolution_SW_elapsed);

	status = initDMAImg(axiDmaImg);
	if(status != XST_SUCCESS){
		printf("\rError: DMA init failed\n");
		return XST_FAILURE;
	}
	printf("\r\nDMA Init done\n\r");

	status = initDMAWeight(axiDmaWeight);
	if(status != XST_SUCCESS){
		printf("\rError: DMA init failed\n");
		return XST_FAILURE;
	}
	printf("\r\nDMA Init done\n\r");

	status = initDMABias(axiDmaBias);
	if(status != XST_SUCCESS){
		printf("\rError: DMA init failed\n");
		return XST_FAILURE;
	}
	printf("\r\nDMA Init done\n\r");

	status = initDoConvolutionSyn(doConvolutionSyn);
	if(status != XST_SUCCESS){
		printf("\rError: DMA init failed\n");
		return XST_FAILURE;
	}
	printf("\r\nDMA Init done\n\r");

    printf("Doing convolution on HW\n");

    // Populate data (Get image from header and put on memory)
    for (int idx = 0; idx < INPUT_SIZE; idx++)
        imgIn_HW[idx] = float2fixed(img_352_resized_hw[idx], 32, 16);

    for (int i = 0; i < WEIGHT_SIZE; i++)
        conv1_w_HW[i] = float2fixed(conv1_w[i], 32, 16);

    for (int i = 0; i < BIAS_SIZE; i++)
        conv1_b_HW[i] = float2fixed(conv1_b[i], 32, 16);

    printf("Starting.... HW\n");
    // Ask for a convolution
    XDoconvolutionsyn_Set_relu(&doConvolutionSyn, false);

    XDoconvolutionsyn_Start(&doConvolutionSyn);

    // Do the DMA transfer to push and get our image
    axiTimer.startTimer();
    Xil_DCacheFlushRange((u32)imgIn_HW, INPUT_SIZE * sizeof(u32));
    Xil_DCacheFlushRange((u32)conv1_w_HW, WEIGHT_SIZE * sizeof(u32));
    Xil_DCacheFlushRange((u32)conv1_b_HW, BIAS_SIZE * sizeof(u32));
    //Xil_DCacheFlushRange((u32)m_dma_buffer_RX, OUTPUT_SIZE * sizeof(float));
    Xil_DCacheFlushRange((u32)m_dma_buffer_RX, OUTPUT_SIZE * sizeof(u32));


    status = XAxiDma_SimpleTransfer(&axiDmaImg, (u32)imgIn_HW, INPUT_SIZE * sizeof(u32), XAXIDMA_DMA_TO_DEVICE);
    if (status != XST_SUCCESS) {
    		//print("Error: DMA transfer to Vivado HLS block failed\n");
    		return XST_FAILURE;
    	}
    status = XAxiDma_SimpleTransfer(&axiDmaWeight, (u32)conv1_w_HW, WEIGHT_SIZE * sizeof(u32), XAXIDMA_DMA_TO_DEVICE);
    if (status != XST_SUCCESS) {
		//print("Error: DMA transfer to Vivado HLS block failed\n");
		return XST_FAILURE;
	}
    status = XAxiDma_SimpleTransfer(&axiDmaBias, (u32)conv1_b_HW, BIAS_SIZE * sizeof(u32), XAXIDMA_DMA_TO_DEVICE);
    if (status != XST_SUCCESS) {
		//print("Error: DMA transfer to Vivado HLS block failed\n");
		return XST_FAILURE;
	}

    // status = XAxiDma_SimpleTransfer(&axiDmaImg, (u32)m_dma_buffer_RX, OUTPUT_SIZE * sizeof(float), XAXIDMA_DEVICE_TO_DMA);
	status = XAxiDma_SimpleTransfer(&axiDmaImg, (u32)m_dma_buffer_RX, OUTPUT_SIZE * sizeof(u32), XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS) {
		//print("Error: DMA transfer from Vivado HLS block failed\n");
		return XST_FAILURE;
	}

    //Wait transfers to finish
	while (XAxiDma_Busy(&axiDmaImg, XAXIDMA_DMA_TO_DEVICE));
	while (XAxiDma_Busy(&axiDmaWeight, XAXIDMA_DMA_TO_DEVICE));
	while (XAxiDma_Busy(&axiDmaBias, XAXIDMA_DMA_TO_DEVICE));

    while (XAxiDma_Busy(&axiDmaImg, XAXIDMA_DEVICE_TO_DMA));

    // Invalidate the cache to avoid reading garbage
    Xil_DCacheInvalidateRange((u32)m_dma_buffer_RX, OUTPUT_SIZE * sizeof(u32));
    axiTimer.stopTimer();

    double HW_elapsed = axiTimer.getElapsedTimerInSeconds();
    printf("HW execution time: %f sec\n", HW_elapsed);

    for (int i = 0; i < 10; i++)
    	printf("Value of res_sw[%d] is %f\n", i, res_sw[i]);
    for (int i = 140784; i < 140800; i++)
    	printf("Value of res_hw[%d] is %f\n", i, fixed2float(m_dma_buffer_RX[i], 32, 16));

    return 0;
}

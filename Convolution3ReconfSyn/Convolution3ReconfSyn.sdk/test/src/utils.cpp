#include <stdio.h>
#include "utils.h"

int initDMAImg(XAxiDma &axiDmaImg)   {
    int status;
    XAxiDma_Config *CfgPtr;
    CfgPtr = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
    if (!CfgPtr)
        printf("Error loading config for axiDmaImg\n");

    status = XAxiDma_CfgInitialize(&axiDmaImg, CfgPtr);
    if (status != XST_SUCCESS)
        printf("Error initializing for axiDmaImg\n");

    // Disable interrupts
    XAxiDma_IntrDisable(&axiDmaImg, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&axiDmaImg, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

    return XST_SUCCESS;
}

int initDMAWeight(XAxiDma &axiDmaWeight)   {
    int status;
    XAxiDma_Config *CfgPtr;
    CfgPtr = XAxiDma_LookupConfig(XPAR_AXI_DMA_1_DEVICE_ID);
    if (!CfgPtr)
        printf("Error loading config for axiDmaWeight\n");

    status = XAxiDma_CfgInitialize(&axiDmaWeight, CfgPtr);
    if (status != XST_SUCCESS)
        printf("Error initializing for axiDmaWeight\n");

    // Disable interrupts
    XAxiDma_IntrDisable(&axiDmaWeight, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&axiDmaWeight, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

    return XST_SUCCESS;
}

int initDMABias(XAxiDma &axiDmaBias)   {
    int status;
    XAxiDma_Config *CfgPtr;
    CfgPtr = XAxiDma_LookupConfig(XPAR_AXI_DMA_2_DEVICE_ID);
    if (!CfgPtr)
        printf("Error loading config for axiDmaBias\n");

    status = XAxiDma_CfgInitialize(&axiDmaBias, CfgPtr);
    if (status != XST_SUCCESS)
        printf("Error initializing for axiDmaBias\n");

    // Disable interrupts
    XAxiDma_IntrDisable(&axiDmaBias, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&axiDmaBias, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

    return XST_SUCCESS;
}

int initDoConvolution3ReconfSyn(XDoconvolution3reconfsyn &doConvolutionSyn)     {
    int status;

    XDoconvolution3reconfsyn_Config *doConvolutionSyn_cfg;
    doConvolutionSyn_cfg = XDoconvolution3reconfsyn_LookupConfig(XPAR_DOCONVOLUTION3RECONFSYN_0_DEVICE_ID);
    if (!doConvolutionSyn_cfg)
        printf("Error loading config for doConvolutionSyn_cfg\n");

    status = XDoconvolution3reconfsyn_CfgInitialize(&doConvolutionSyn, doConvolutionSyn_cfg);
    if (status != XST_SUCCESS)
        printf("Error initializing for doConvolutionSyn\n");

    return status;
}

unsigned int float_to_u32 (float val)
{
	unsigned int result;
	union float_bytes	{
		float v;
		unsigned char bytes[4];
	} data;
	data.v =val;
	/*for (int i = 0; i < 4; i++)
		printf("Byte %d : %.2x\n", i, data.bytes[i]);*/
	result = (data.bytes[3] << 24) + (data.bytes[2] << 16) + (data.bytes[1] << 8) + (data.bytes[0]);
	return result;
}

float u32_to_float (unsigned int val)	{
	union {
		float val_float;
		unsigned char bytes[4];
	} data;
	data.bytes[3] = (val >> (8 * 3)) & 0xff;
	data.bytes[2] = (val >> (8 * 2)) & 0xff;
	data.bytes[1] = (val >> (8 * 1)) & 0xff;
	data.bytes[0] = (val >> (8 * 0)) & 0xff;

	return data.val_float;
}

unsigned int float2fixed(float num, int totalBits, int integerBits)   {
    float pos = num;
    if (pos < 0)
        pos = 0 - pos;

    int floatBits = totalBits - integerBits;
    unsigned int result = 0;
    int intPart = (int)pos;
    float floatPart = pos - intPart;

    for (int i = 0; intPart > 0; i++) {
        if ((intPart & 1) == 1)   {
            result |= (1 << (i + floatBits));
        }
        intPart = (intPart >> 1);
    }

    for (int i = 0; i < floatBits; i++) {
        floatPart *= 2;
        if (floatPart >= 1)   {
            result |= (1 << (floatBits - 1 - i));
            floatPart--;
        }
    }
    if (num < 0)
        return (~result + 1);
    return result;
}

float fixed2float(unsigned int num, int totalBits, int integerBits)   {
    bool neg = false;
    unsigned int pos = num;
    if (((pos >> (totalBits - 1)) & 1) == 1)    {
        neg = true;
        pos = ~num + 1;
    }

    int floatBits = totalBits - integerBits;
    float result = 0;

    for (int i = 0; i < floatBits; i++)     {
        if ((pos & 1) == 1)     {
            float toAdd = 1;
            for (int j = 0; j < floatBits - i; j++)
                toAdd /= 2;
            result += toAdd;
        }
        pos = pos >> 1;
    }

    for (int i = floatBits; i < totalBits; i++)     {
        if ((pos & 1) == 1)     {
            result += (1 << (i - floatBits));
        }
        pos = pos >> 1;
    }
    if (neg)
        return 0 - result;
    return result;
}

void printBits(size_t const size, void const * const ptr)	{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = b[i] & (1<<j);
            byte >>= j;
            printf("%u", byte);
        }
    }
    puts("");
}

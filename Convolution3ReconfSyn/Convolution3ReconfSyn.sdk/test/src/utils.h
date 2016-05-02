/*
 * utils.h
 *
 *  Created on: 2016/4/28
 *      Author: anson
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "xaxidma.h"
#include "xdoconvolution3reconfsyn.h"

int initDMAImg(XAxiDma &axiDmaImg);
int initDMAWeight(XAxiDma &axiDmaWeight);
int initDMABias(XAxiDma &axiDmaBias);
int initDoConvolution3ReconfSyn(XDoconvolution3reconfsyn &doConvolutionSyn);

unsigned int float_to_u32 (float val);
float u32_to_float (unsigned int val);
unsigned int float2fixed(float num, int totalBits, int integerBits);
float fixed2float(unsigned int num, int totalBits, int integerBits);
void printBits(size_t const size, void const * const ptr);

#endif /* UTILS_H_ */

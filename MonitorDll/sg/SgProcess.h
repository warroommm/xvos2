//==============================================================================
//
// Title:		SgProcess
// Purpose:		A short description of the interface.
//
// Created on:	2013/10/22 at 21:22:55 by DclChene.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __SgProcess_H__
#define __SgProcess_H__

#ifdef __cplusplus
    extern "C" {
#endif

#define LOWPASS				0 			//低通     
#define HIGHPASS    		1			//高通
#define BANDPASS			2			//带通
#define BANDSTOP			3			//带阻

#define RECTANGLE			0			//矩形窗
#define TRIANGLE			30			//三角形窗
#define HANNING				1			//汉宁窗
#define HAMMING				2			//海明窗
#define BLKMAN				5			//布拉克曼窗
#define KAISER				60			//凯塞窗

typedef struct ComplexNumber_struct {
    double real;
    double imaginary;
}COMPLEX_NUMBER;

//创建FFT表
void * __stdcall CreateFftTable (int TableSize);
//销毁FFT表
void __stdcall DestroyFftTable (void *FftTable);
//FFTEx运算
void __stdcall ComputeFFTEx (double *pReal, int ElemNum, int FftNum, void *pTable, COMPLEX_NUMBER *pFFTComplex);
//FFTEx逆运算	                                                    
void __stdcall ComputeInvFFTEx (COMPLEX_NUMBER *pFFTComplex, int FftNum, void *pTable, double *pReal);
//FFT运算
void __stdcall ComputeFFT (double *pReal, double *pImaginary, int ElemNum);
//FFT逆运算
void __stdcall ComputeInvFFT (double *pReal, double *pImaginary, int ElemNum);
//生产滤波系数
void __stdcall FilterCoef (int SampleFreq, int LowCutFreq, int FilterLen, double *pCoef);

//FIR窄带数字滤波器
//pData 		— 待滤波的数据组
//inSize 		— 待滤波的数据量
//FilterType 	— 滤波器类型
//SampleFreq 	— 信号采样频率
//LowCutFreq 	— 低通滤波时的上限截止频率或带通滤波时的下限截止频率
//HighCutFreq 	— 高通滤波时的下限截止频率或带通滤波时的上限截止频率
int  __stdcall FirNarrowBandFilter (double *pData, int inSize, int FilterType, int SampleFreq, 
						  			int LowCutFreq, int HighCutFreq);
//产生白噪声
void __stdcall GenerateWhiteNoise (int Num, double Amplitude, int Seed, double *pNoise);

//加窗
void __stdcall AddWindow (int WinType, double *pData, int DataSize);
void __stdcall ReSample (double *pIn, int SizeIn, int Factor, double *pOut);
		
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __SgProcess_H__ */

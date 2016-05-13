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

#define LOWPASS				0 			//��ͨ     
#define HIGHPASS    		1			//��ͨ
#define BANDPASS			2			//��ͨ
#define BANDSTOP			3			//����

#define RECTANGLE			0			//���δ�
#define TRIANGLE			30			//�����δ�
#define HANNING				1			//������
#define HAMMING				2			//������
#define BLKMAN				5			//����������
#define KAISER				60			//������

typedef struct ComplexNumber_struct {
    double real;
    double imaginary;
}COMPLEX_NUMBER;

//����FFT��
void * __stdcall CreateFftTable (int TableSize);
//����FFT��
void __stdcall DestroyFftTable (void *FftTable);
//FFTEx����
void __stdcall ComputeFFTEx (double *pReal, int ElemNum, int FftNum, void *pTable, COMPLEX_NUMBER *pFFTComplex);
//FFTEx������	                                                    
void __stdcall ComputeInvFFTEx (COMPLEX_NUMBER *pFFTComplex, int FftNum, void *pTable, double *pReal);
//FFT����
void __stdcall ComputeFFT (double *pReal, double *pImaginary, int ElemNum);
//FFT������
void __stdcall ComputeInvFFT (double *pReal, double *pImaginary, int ElemNum);
//�����˲�ϵ��
void __stdcall FilterCoef (int SampleFreq, int LowCutFreq, int FilterLen, double *pCoef);

//FIRխ�������˲���
//pData 		�� ���˲���������
//inSize 		�� ���˲���������
//FilterType 	�� �˲�������
//SampleFreq 	�� �źŲ���Ƶ��
//LowCutFreq 	�� ��ͨ�˲�ʱ�����޽�ֹƵ�ʻ��ͨ�˲�ʱ�����޽�ֹƵ��
//HighCutFreq 	�� ��ͨ�˲�ʱ�����޽�ֹƵ�ʻ��ͨ�˲�ʱ�����޽�ֹƵ��
int  __stdcall FirNarrowBandFilter (double *pData, int inSize, int FilterType, int SampleFreq, 
						  			int LowCutFreq, int HighCutFreq);
//����������
void __stdcall GenerateWhiteNoise (int Num, double Amplitude, int Seed, double *pNoise);

//�Ӵ�
void __stdcall AddWindow (int WinType, double *pData, int DataSize);
void __stdcall ReSample (double *pIn, int SizeIn, int Factor, double *pOut);
		
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __SgProcess_H__ */

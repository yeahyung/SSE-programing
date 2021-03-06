#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <Windows.h>
#include<xmmintrin.h>
#include<emmintrin.h>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
	byte gray = 0, r, g, b, rr, gg, bb;
	__m128i xmm, xmm1, xmm2, xmm3, R, bright, add, real, tt, result1, sero, serott, xmm4, xmm5, xmm6;
	Mat temp = imread("k2.jpg");
	Mat image = imread("k1.jpg");
	Mat a1, a2, a3, a4;
	int cols = image.cols, rows = image.rows;
	int length = cols * rows * 3;
	int t;

	byte *tempw = new byte[length * 4];
	//byte *tempw = new byte[length/4];
	byte* data = (byte*)image.data;
	byte* data2 = (byte*)temp.data;
	float sp_time;
	int64 f, t1, t2, f1, tt1, tt2;
	while (1) {
		scanf_s("%d", &t);
		switch (t)
		{
		case 1: {	//이미지 합치기
			QueryPerformanceFrequency((LARGE_INTEGER *)&f);
			QueryPerformanceCounter((LARGE_INTEGER *)&t1);

			for (int i = 0; i < length; i += 16) {
				xmm = _mm_loadu_si128((__m128i*)(data + i));
				xmm1 = _mm_loadu_si128((__m128i*)(data2 + i));
				R = _mm_avg_epu8(xmm, xmm1);
				_mm_storeu_si128((__m128i*)(tempw + i), R);
			}
			QueryPerformanceCounter((LARGE_INTEGER *)&t2);
			sp_time = (float)(t2 - t1) / f * 1000;
			printf("SIMD : %fms\n", sp_time);
			Mat a1(rows, cols, CV_8UC3, tempw);
			imwrite("addImage.jpg", a1);
			imshow("SIMD", a1);
			waitKey(0);
			break; }
		case 2: {		// 밝기
			QueryPerformanceFrequency((LARGE_INTEGER *)&f);
			QueryPerformanceCounter((LARGE_INTEGER *)&t1);

			bright = _mm_set1_epi8(100);
			for (int i = 0; i < length; i += 16) {
				xmm = _mm_loadu_si128((__m128i*)(data + i));
				//R = _mm_add_epi8(xmm, bright);
				R = _mm_adds_epu8(xmm, bright);
				//	R = _mm_subs_epu8(xmm, bright);
				_mm_storeu_si128((__m128i*)(tempw + i), R);
			}
			QueryPerformanceCounter((LARGE_INTEGER *)&t2);
			sp_time = (float)(t2 - t1) / f * 1000;
			printf("SIMD : %fms\n", sp_time);
			Mat a2(rows, cols, CV_8UC3, tempw);
			imwrite("bright.jpg", a2);
			imshow("SIMD", a2);
			waitKey(0);
			break; }
		case 3: {	//이미지 2배 확대
			QueryPerformanceFrequency((LARGE_INTEGER *)&f);
			QueryPerformanceCounter((LARGE_INTEGER *)&t1);

			for (int i = 0; i < rows - 1; i++) {
				for (int j = 0; j < cols * 3; j += 16) {
					int temp = i * cols * 3;
					xmm = _mm_loadu_si128((__m128i*)(data + j + temp));
					xmm1 = _mm_loadu_si128((__m128i*)(data + j + 3 + temp));
					xmm2 = _mm_avg_epu8(xmm, xmm1);
					xmm3 = _mm_unpacklo_epi8(xmm, xmm2);
					xmm5 = _mm_unpackhi_epi8(xmm, xmm2);
					_mm_storeu_si128((__m128i*)(tempw + j * 2 + temp * 4), xmm3);
					_mm_storeu_si128((__m128i*)(tempw + j * 2 + temp * 4 + 16), xmm5);

					sero = _mm_loadu_si128((__m128i*)(data + j + temp + cols * 3));
					serott = _mm_avg_epu8(xmm, sero);
					xmm4 = _mm_unpacklo_epi8(xmm, serott);
					xmm6 = _mm_unpackhi_epi8(xmm, serott);
					_mm_storeu_si128((__m128i*)(tempw + j * 2 + temp * 4 + cols * 6), xmm4);
					_mm_storeu_si128((__m128i*)(tempw + j * 2 + temp * 4 + cols * 6 + 16), xmm6);
				}
			}
			QueryPerformanceCounter((LARGE_INTEGER *)&t2);
			sp_time = (float)(t2 - t1) / f * 1000;
			printf("SIMD : %fms\n", sp_time);

			Mat a3(rows * 2, cols * 2, CV_8UC3, tempw);
			imwrite("big.jpg", a3);
			imshow("SIMD", a3);
			waitKey(0);
			break; }
		case 4: {	//이미지 2배축소
			
			QueryPerformanceFrequency((LARGE_INTEGER *)&f);
			QueryPerformanceCounter((LARGE_INTEGER *)&t1);

			R = _mm_set_epi8(0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255);
			for (int i = 0; i < rows; i += 2) {
				for (int j = 0; j < cols * 3 - 16; j += 32) {
					xmm = _mm_loadu_si128((__m128i*)(data + j + i * cols * 3));
					xmm1 = _mm_and_si128(xmm, R);

					xmm2 = _mm_loadu_si128((__m128i*)(data + j + 16 + i * cols * 3));
					xmm3 = _mm_and_si128(xmm2, R);
					result1 = _mm_packus_epi16(xmm1, xmm3);
					_mm_storeu_si128((__m128i*)(tempw + j / 2 + i / 2 * cols / 2 * 3), result1);
				}
			}
			QueryPerformanceCounter((LARGE_INTEGER *)&t2);
			sp_time = (float)(t2 - t1) / f * 1000;
			printf("SIMD : %fms\n", sp_time);

			Mat a4(rows / 2, cols / 2, CV_8UC3, tempw);
			imwrite("small.jpg", a4);
			imshow("SIMD", a4);
			waitKey(0);
			break;
		}
		case 5: //최대값 출력
		{
			byte num[192] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
				1,2,3,4,5,6,7,200,190,10,11,12,13,14,15,16,1,2,3,4,5,6,177,8,9,123,11,112,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
			for (int i = 0; i < 192; i += 16) {
				xmm = _mm_loadu_si128((__m128i*)(num));
				xmm1 = _mm_loadu_si128((__m128i*)(num + i));
				xmm2 = _mm_max_epu8(xmm, xmm1);
				_mm_storeu_si128((__m128i*)(num), xmm2);
			}
			byte max = 0;
			for (int i = 0; i < 16; i++) {
				if (max < num[i])
					max = num[i];
			}
			/*byte max = 0;
			for (int i = 0; i < 192; i++)
				if (max < num[i])
					max = num[i];*/
			printf("max = %d\n", max);

			break;
		} // case문
		case 6: // 홀수 짝수 찾기
		{

			byte num[32] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32 };
			__m128i RR,result,odd;
			RR = _mm_set1_epi8(1);
			tt = _mm_set1_epi8(0);
			for (int i = 0; i < 32; i += 16) {
				xmm = _mm_loadu_si128((__m128i*)(num + i));
				R = _mm_and_si128(xmm, RR);
				//result = _mm_subs_epi8(tt, R); // 홀수찾기
				result = _mm_subs_epi16(R,RR); //짝수찾기
				odd = _mm_and_si128(xmm, result);
				_mm_storeu_si128((__m128i*)(num + i), odd);
			}
			for (int i = 0; i < 32; i++)
				if(num[i]!=0)
				printf("%d ", num[i]);

			break;
		}
		case 7: // x초과 y미만 숫자만 출력하기
		{

			byte num[32] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32 };
			__m128i first, second,firstResult,secondResult,tempResult;
			first = _mm_set1_epi8(2);
			second = _mm_set1_epi8(20);
			for (int i = 0; i < 32; i += 16) {
				xmm=_mm_loadu_si128((__m128i*)(num + i));
				firstResult = _mm_cmpgt_epi8(xmm, first);
				secondResult = _mm_cmplt_epi8(xmm, second);
				tempResult = _mm_and_si128(firstResult, secondResult);
				xmm = _mm_and_si128(xmm, tempResult);
				_mm_storeu_si128((__m128i*)(num + i), xmm);
			}	
			for (int i = 0; i < 32; i++)
				if(num[i]!=0)
				printf("%d ", num[i]);

			break;
		}
		case 8:  // shuffle
		{
			int num[32] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32 };
			for (int i = 0; i < 32; i += 4) {
				xmm = _mm_loadu_si128((__m128i*)(num + i));
				xmm1 = _mm_shuffle_epi32(xmm, 75); // 75 = 01 00 10 11 뒤에서부터 11(2진수) =3이므로 3번째 인덱스의 값(4)를 첫번째 위치에
												   // 놓고 10=2 2번쨰 인덱스의 값(3)을 2번째 위치에, 00=0 0번째 인덱스의 값(1)을 3번쨰 위치에...
				_mm_storeu_si128((__m128i*)(num + i), xmm1);
			}
			for (int i = 0; i < 32; i++)
				printf("%d ", num[i]);
		}
		case 9: // 나누기 
		{
			int num[32] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32 };
			byte num1[32] = { 0,0,0,8,0,0,0,0,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32 };
			float result[32] = { 0, };
			byte result1[32] = { 0, };
			__m128d xmm7,xmm8;
			xmm1 = _mm_set1_epi32(1);
			xmm2 = _mm_set1_epi8(3);
			for (int i = 0; i < 32; i += 4) {
				xmm = _mm_loadu_si128((__m128i*)(num + i));
				//xmm2 = _mm_srli_epi32(xmm, 2);  //   * or / 2^n
				xmm3 = _mm_castps_si128(_mm_div_ps(_mm_castsi128_ps(xmm), _mm_castsi128_ps(xmm1)));
				_mm_storeu_si128((__m128i*)(result + i), xmm3);
			}
			for (int i = 0; i < 32; i += 16) {
				xmm = _mm_loadu_si128((__m128i*)(num1 + i));
				xmm3 = _mm_castps_si128(_mm_div_ps(_mm_castsi128_ps(xmm), _mm_castsi128_ps(xmm1)));
				_mm_storeu_si128((__m128i*)(result1 + i), xmm3);
			}
			for (int i = 0; i < 32; i++)
				//printf("%f ", result[i]);
				printf("%d ", result1[i]);
		}
		} // switch문
	}








	// 상 반전
	/*bright = _mm_set1_epi8(255);
	for (int i = 0; i < length; i += 16) {
	xmm = _mm_loadu_si128((__m128i*)(data + i));
	R = _mm_andnot_si128(xmm, bright);
	_mm_storeu_si128((__m128i*)(tempw + i), R);
	}
	QueryPerformanceCounter((LARGE_INTEGER *)&t2);
	float sp_time = (float)(t2 - t1) / f * 1000;
	printf("SIMD : %fms\n", sp_time);

	Mat temppp1(rows, cols, CV_8UC3, tempw);
	imwrite("reverse.jpg", temppp1);
	imshow("SIMD", temppp1);
	waitKey(0);
	*/
	return 0;
}
byte xClip(double x) {
	if (x < 0) return 0;
	if (x > 255) return 255;
	return (byte)x;
}

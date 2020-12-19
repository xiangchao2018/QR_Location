#ifndef _OPENCV_TEST_H_
#define _OPENCV_TEST_H_
#include"common.h"

const int g_nMaxAlphaValue = 100;
int g_nAlphaValueSlider;
double g_dAlphaValue;
double g_dBetaValue;

Mat g_srcImage1;
Mat g_srcImage2;
Mat g_Image1;
Mat g_Image2;
Mat g_dstImage;

void on_TrackBar(int, void*)
{
	g_dAlphaValue = (double)g_nAlphaValueSlider / g_nMaxAlphaValue;

	g_dBetaValue = 1 - g_dAlphaValue;

	addWeighted(g_Image1, g_dAlphaValue, g_Image2, g_dBetaValue, 0.0, g_dstImage);

	imshow("dst", g_dstImage);
}



#endif // !1




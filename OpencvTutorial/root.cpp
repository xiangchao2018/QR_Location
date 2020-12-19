#include<iostream>
#include<string.h>
#include"ImageProcess.h"
#include"ImageEnhance.h"
#include"opencvtest.h"
#include"QRLocation.h"
using namespace std;
# pragma warning (disable:4819)



void test_trackbar()
{
	g_srcImage1 = imread("butterfly.jpg");
	g_srcImage2 = imread("lena.jpg");
	Size dsize = Size(500, 500);
	g_Image1 = Mat(dsize, CV_32S);
	g_Image2 = Mat(dsize, CV_32S);

	resize(g_srcImage1, g_Image1,dsize);
	resize(g_srcImage2, g_Image2, dsize);

	g_nAlphaValueSlider = 70;

	namedWindow("dst", 1);

	char TrackBarName[50];
	sprintf_s(TrackBarName, "透明度%d", g_nMaxAlphaValue);
	createTrackbar(TrackBarName,"dst",&g_nAlphaValueSlider,g_nMaxAlphaValue,on_TrackBar);
	
	on_TrackBar(g_nAlphaValueSlider, 0);
	waitKey(0);
}

void  test_QR_location()
{
	QRLocation* pQRDetectImpl=new QRLocation("timg.jpg");
	
	pQRDetectImpl->QRLoctionTest();
	waitKey(0);
}

int add(int a)
{
	return a +0;
}

int main()
{
	/*ImageProcess IP("HappyFish.jpg");
	IP.ShowImage(IP.GetImageMat(),"output");
	cv::waitKey(0);*/
	/*ImageEnhance IE;
	IE.TestImageEnhance("darkPicture.jpg");*/
	//test_trackbar();
	
	test_QR_location();

	system("pause");
	return 0;
}
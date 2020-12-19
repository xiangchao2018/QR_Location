#include "ImageEnhance.h"



ImageEnhance::ImageEnhance()
{
}


ImageEnhance::~ImageEnhance()
{
}

void ImageEnhance::TestImageEnhance(string filePath)
{
	Mat src=imread(filePath);
	Mat copySrc = src.clone();
	imshow("output_src", src);
	//waitKey(0);
	/************��1������ֱ��ͼ���о��⻯***************/
	Mat imageRGB[3];
	split(copySrc, imageRGB);
	for (int ii=0;ii<3;ii++)
	{
		equalizeHist(imageRGB[ii], imageRGB[ii]);
	}
	merge(imageRGB, 3, copySrc);
	imshow("equalizeList_src", copySrc);
	/************��2������laplace���ӽ�����ǿ***************/
	Mat laplaceSrc = src.clone();
	Mat kernel = (Mat_<float>(3,3)<<0,-1,0,0,5,0,0,-1,0);
	filter2D(src, laplaceSrc, CV_8UC3, kernel);
	imshow("laplace_src", laplaceSrc);
	
	/************��3�����ڶ���Log�任��ͼ����ǿ***************/
	Mat imageLog(src.size(), CV_32FC3);
	for (int ii=0;ii<src.rows;ii++)
	{
		for (int jj=0;jj<src.cols;jj++)
		{
			imageLog.at<Vec3f>(ii, jj)[0] = log(1 + src.at<Vec3b>(ii, jj)[0]);
			imageLog.at<Vec3f>(ii, jj)[1] = log(1 + src.at<Vec3b>(ii, jj)[1]);
			imageLog.at<Vec3f>(ii, jj)[2] = log(1 + src.at<Vec3b>(ii, jj)[2]);
		}
	}
	normalize(imageLog, imageLog,0,255, NORM_MINMAX);
	convertScaleAbs(imageLog, imageLog);
	imshow("imageLog", imageLog);

	/************��4������٤��任��ͼ����ǿ***************/
	Mat imageGamma(src.size(),CV_32FC3);
	for (int ii=0;ii<imageGamma.rows;ii++)
	{
		for (int jj = 0; jj<imageGamma.cols; jj++)
		{
			imageGamma.at<Vec3f>(ii,jj)[0] = (src.at<Vec3b>(ii, jj)[0])*(src.at<Vec3b>(ii, jj)[0])*(src.at<Vec3b>(ii, jj)[0]);
			imageGamma.at<Vec3f>(ii, jj)[1]= (src.at<Vec3b>(ii, jj)[1])*(src.at<Vec3b>(ii, jj)[1])*(src.at<Vec3b>(ii, jj)[1]);
			imageGamma.at<Vec3f>(ii, jj)[2] = (src.at<Vec3b>(ii, jj)[2])*(src.at<Vec3b>(ii, jj)[2])*(src.at<Vec3b>(ii, jj)[2]);
		}
	}
	normalize(imageGamma, imageGamma, 0, 255, NORM_MINMAX);
	convertScaleAbs(imageGamma, imageGamma);
	imshow("imageGamma", imageGamma);
	waitKey(0);
}

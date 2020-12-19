#ifndef _IMAGEPROCESS_H_
#define _IMAGEPROCESS_H_
#include<iostream>
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include"opencv2\imgproc\imgproc.hpp"

enum ThresholdType
{
	ADAPTIVE_THRESH_MEAN_C=0,
	ADAPTIVE_THRESH_GAUSSIAN_C
};


class ImageProcess
{
public:
	ImageProcess(std::string filePath);
	//展示图片
	void ShowImage(cv::Mat &img,std::string strWindowName);
	//保存图片
	void SaveImageToDisk(cv::Mat &img, std::string strSavePath);
	
	//获取矩阵
	cv::Mat GetImageMat();

private:
	//图片预处理
	int PreProcess(cv::Mat &img);


	//自适应阈值到二值图像
	void ThresholdValueProcess(cv::Mat src, cv::Mat  &dst, double maxValue,
		int adaptiveMethod, int thresholdType, int blockSize, double C);

	//canny算子检测轮廓
	void CannyProcess();


private:
	cv::Mat m_image;
};

#endif // !1


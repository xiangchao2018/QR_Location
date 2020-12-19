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
	//չʾͼƬ
	void ShowImage(cv::Mat &img,std::string strWindowName);
	//����ͼƬ
	void SaveImageToDisk(cv::Mat &img, std::string strSavePath);
	
	//��ȡ����
	cv::Mat GetImageMat();

private:
	//ͼƬԤ����
	int PreProcess(cv::Mat &img);


	//����Ӧ��ֵ����ֵͼ��
	void ThresholdValueProcess(cv::Mat src, cv::Mat  &dst, double maxValue,
		int adaptiveMethod, int thresholdType, int blockSize, double C);

	//canny���Ӽ������
	void CannyProcess();


private:
	cv::Mat m_image;
};

#endif // !1


#include "ImageProcess.h"

ImageProcess::ImageProcess(std::string  filePath)
{
	m_image = cv::imread(filePath);
}

void ImageProcess::ShowImage(cv::Mat & img,std::string strWindowName)
{
	cv::imshow(strWindowName, img);
}

void ImageProcess::SaveImageToDisk(cv::Mat & img, std::string strSavePath)
{
	cv::imwrite(strSavePath, img);
}

cv::Mat ImageProcess::GetImageMat()
{
	return m_image;
}

int ImageProcess::PreProcess(cv::Mat & img)
{
	return 0;
}

void ImageProcess::ThresholdValueProcess(cv::Mat src, cv::Mat & dst, double maxValue, int adaptiveMethod, int thresholdType, int blockSize, double C)
{
	cv::adaptiveThreshold(src,dst,maxValue,adaptiveMethod,thresholdType,blockSize,C);
}

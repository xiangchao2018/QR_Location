#ifndef _QRLOCATION_H_
#define _QRLOCATION_H_

#include<iostream>
#include<vector>
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include"opencv2\imgproc\imgproc.hpp"

using namespace std;
using namespace cv;

class QRLocation
{
public:
	QRLocation(const std::string filePath);
	~QRLocation();
	void QRLoctionTest();
private:
	//确定三角关系中的直角
	int LeftTopPoint(vector<Point> centerPoints);
	//确定另外两个回的位置
	vector<int> OtherTwoPoint(vector<Point> centerPoints,int leftTopPointIndex);
	double rotateAngle(Point leftTopPoint, Point rightTopPoint, Point leftBottomPoint);
	
	// 该部分用于检测是否是角点
	bool IsQrPoint(vector<Point>& contour, Mat& img);

	//用于判断是否属于角上的正方形
	bool isCorner(Mat &image);
	double Rate(Mat &count);

	Mat transformCorner(Mat src, RotatedRect rect);
	Mat transformQRcode(Mat src, RotatedRect rect, double angle);
private:
	cv::Mat m_srcMat;
	cv::Mat m_srcMatCopy;
	cv::Mat m_canvas;   //canvas为画布 将找到的定位特征画出来
	cv::Mat m_srcGray;  //灰度图
	vector<cv::Point> m_centers;

};
#endif // !_QRLOCATION_H_

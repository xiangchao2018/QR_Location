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
	//ȷ�����ǹ�ϵ�е�ֱ��
	int LeftTopPoint(vector<Point> centerPoints);
	//ȷ�����������ص�λ��
	vector<int> OtherTwoPoint(vector<Point> centerPoints,int leftTopPointIndex);
	double rotateAngle(Point leftTopPoint, Point rightTopPoint, Point leftBottomPoint);
	
	// �ò������ڼ���Ƿ��ǽǵ�
	bool IsQrPoint(vector<Point>& contour, Mat& img);

	//�����ж��Ƿ����ڽ��ϵ�������
	bool isCorner(Mat &image);
	double Rate(Mat &count);

	Mat transformCorner(Mat src, RotatedRect rect);
	Mat transformQRcode(Mat src, RotatedRect rect, double angle);
private:
	cv::Mat m_srcMat;
	cv::Mat m_srcMatCopy;
	cv::Mat m_canvas;   //canvasΪ���� ���ҵ��Ķ�λ����������
	cv::Mat m_srcGray;  //�Ҷ�ͼ
	vector<cv::Point> m_centers;

};
#endif // !_QRLOCATION_H_

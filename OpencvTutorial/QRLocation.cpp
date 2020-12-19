#include "QRLocation.h"


QRLocation::QRLocation(const std::string filePath)
{
	m_srcMat = cv::imread(filePath, cv::ImreadModes::IMREAD_COLOR);
	

	m_srcMatCopy = m_srcMat.clone();
	m_canvas = Mat::zeros(m_srcMat.size(), CV_8UC3);

	cvtColor(m_srcMat, m_srcGray, COLOR_BGR2GRAY);

	Canny(m_srcGray,m_srcGray,50,150,3);

	Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(m_srcGray, m_srcGray, MORPH_CLOSE, erodeStruct);
}

QRLocation::~QRLocation()
{
}

void QRLocation::QRLoctionTest()
{
	/*contours�ǵ�һ��Ѱ������*/
	/*contours2��ɸѡ��������*/
	vector<vector<cv::Point>> contours;
	//	�����������
	vector<Vec4i> hierarchy;

	findContours(m_srcGray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	//��¼С���������
	int numOfRects = 0;
	//��ⷽ��
	int ic = 0;
	int parentIdx = -1;
	for (uint ii = 0; ii<contours.size(); ii++)
	{
		if (hierarchy[ii][2] != -1 && ic == 0)
		{
			parentIdx = ii;
			ic++;
		}
		else if (hierarchy[ii][2] != -1)
		{
			ic++;
		}
		else if (hierarchy[ii][2] == -1)
		{
			parentIdx = -1;
			ic = 0;
		}
		if (ic == 5)
		{
			if (IsQrPoint(contours[parentIdx], m_srcMat))
			{
				RotatedRect rect = minAreaRect(Mat(contours[parentIdx]));
				//��ͼ����
				Point2f points[4];
				rect.points(points);
				for (int jj = 0; jj<4; jj++)
				{
					line(m_srcMat, points[jj], points[(jj + 1) % 4], Scalar(0, 255, 0), 2);
				}

				drawContours(m_canvas, contours, parentIdx, Scalar(0, 0, 255), -1);
				m_centers.push_back(rect.center);
				numOfRects++;
			}
			ic = 0;
			parentIdx = -1;
		}
	}

	//��������������
	for (uint ii=0;ii<m_centers.size();ii++)
	{
		line(m_canvas,m_centers[ii],m_centers[(ii+1)% m_centers.size()],Scalar(255,0,0),3);
	}
	vector<vector<Point>> contours3;
	Mat canvasGray;
	cvtColor(m_canvas, canvasGray, COLOR_BGR2GRAY);
	findContours(canvasGray, contours3, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<Point> maxContours;
	double maxArea = 0;
	// ��ԭͼ�л�����ά�������
	for (int i = 0; i < contours3.size(); i++)
	{
		RotatedRect rect = minAreaRect(contours3[i]);
		Point2f boxpoint[4];
		rect.points(boxpoint);
		for (int i = 0; i < 4; i++)
			line(m_srcMat, boxpoint[i], boxpoint[(i + 1) % 4], Scalar(0, 0, 255), 3);

		double area = contourArea(contours3[i]);
		if (area > maxArea) {
			maxContours = contours3[i];
			maxArea = area;
		}
	}
	// ���㡰�ء��Ĵ����ϵ
	if (m_centers.size()!=3)
	{
		cout << m_centers.size()<<endl;
		cout << "not find QRcode..."<<endl;
		return;
	}
	int leftTopPointIndex = LeftTopPoint(m_centers);
	vector<int> otherTwoPointIndex = OtherTwoPoint(m_centers, leftTopPointIndex);
	// canvas�ϱ�ע�������ء��Ĵ����ϵ
	circle(m_canvas, m_centers[leftTopPointIndex], 10, Scalar(255, 0, 255), -1);
	circle(m_canvas, m_centers[otherTwoPointIndex[0]], 10, Scalar(0, 255, 0), -1);
	circle(m_canvas, m_centers[otherTwoPointIndex[1]], 10, Scalar(0, 255, 255), -1);
	// ������ת��
	double angle = rotateAngle(m_centers[leftTopPointIndex], m_centers[otherTwoPointIndex[0]], m_centers[otherTwoPointIndex[1]]);

	// �ó�֮ǰ�õ�����������
	RotatedRect rect = minAreaRect(Mat(maxContours));
	Mat image = transformQRcode(m_srcMatCopy, rect, angle);

	cv::imshow("src", image);
	cv::imshow("canvas", m_canvas);
}

int QRLocation::LeftTopPoint(vector<Point> centerPoints)
{
	int16_t minIndex = 0;
	int16_t multiple = 0;
	int16_t minMutiple = 10000;
	multiple = (centerPoints[1].x- centerPoints[0].x)*(centerPoints[2].x - centerPoints[0].x) - (centerPoints[1].y - centerPoints[0].y)*(centerPoints[2].y - centerPoints[0].y);
	if (minMutiple>multiple)
	{
		minIndex = 0;
		minMutiple = multiple;
	}
	multiple = (centerPoints[2].x - centerPoints[1].x)*(centerPoints[0].x - centerPoints[1].x) - (centerPoints[2].y - centerPoints[1].y)*(centerPoints[0].y - centerPoints[1].y);
	if (minMutiple>multiple)
	{
		minIndex = 1;
		minMutiple = multiple;
	}
	multiple = (centerPoints[1].x - centerPoints[2].x)*(centerPoints[0].x - centerPoints[2].x) - (centerPoints[0].y - centerPoints[2].y)*(centerPoints[1].y - centerPoints[2].y);
	if (minMutiple>multiple)
	{
		minIndex = 2;
		minMutiple = multiple;
	}

	return minIndex;
}

vector<int> QRLocation::OtherTwoPoint(vector<Point> centerPoints, int leftTopPointIndex)
{
	vector<int> vecOtherIndex;
	double waiji = (centerPoints[(leftTopPointIndex+1)%3].x- centerPoints[(leftTopPointIndex) % 3].x)*(centerPoints[(leftTopPointIndex + 2) % 3].y - centerPoints[(leftTopPointIndex) % 3].y)-(centerPoints[(leftTopPointIndex + 2) % 3].x - centerPoints[(leftTopPointIndex) % 3].x)*(centerPoints[(leftTopPointIndex + 1) % 3].y - centerPoints[(leftTopPointIndex) % 3].y);
	if (waiji>0)
	{
		vecOtherIndex.push_back((leftTopPointIndex + 1) % 3);
		vecOtherIndex.push_back((leftTopPointIndex + 2) % 3);
	}
	else
	{
		vecOtherIndex.push_back((leftTopPointIndex +2) % 3);
		vecOtherIndex.push_back((leftTopPointIndex +1) % 3);
	}


	return vecOtherIndex;
}

double QRLocation::rotateAngle(Point leftTopPoint, Point rightTopPoint, Point leftBottomPoint)
{
	double dy = rightTopPoint.y - leftTopPoint.y;
	double dx = rightTopPoint.x - leftTopPoint.x;
	double k = dy / dx;
	double angle = atan(k) * 180 / CV_PI;//ת���Ƕ�
	if (leftBottomPoint.y < leftTopPoint.y)
		angle -= 180;
	return angle;
}

bool QRLocation::IsQrPoint(vector<Point>& contour, Mat & img)
{
	double area = contourArea(contour);
	if (area < 30)
		return false;
	RotatedRect rect = minAreaRect(Mat(contour));
	double w = rect.size.width;
	double h = rect.size.height;
	double rate = min(w, h) / max(w,h);
	if (rate > 0.7)
	{
		Mat image = transformCorner(img, rect);
		if (isCorner(image))
		{
			return 1;
		}
	}
	return false;
}

bool QRLocation::isCorner(Mat & image)
{
	//����mask
	Mat imgCopy, dstCopy;
	Mat dstGray;
	imgCopy = image.clone();

	// ת��Ϊ�Ҷ�ͼ��
	cvtColor(image, dstGray, COLOR_BGR2GRAY);
	// ���ж�ֵ��
	threshold(dstGray, dstGray, 0, 255, THRESH_BINARY | THRESH_OTSU);

	dstCopy = dstGray.clone();  //����

	Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(dstCopy, dstCopy, MORPH_OPEN, erodeStruct);
	// �ҵ������봫�ݹ�ϵ
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dstCopy, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++)
	{
		//cout << i << endl;
		if (hierarchy[i][2] == -1 && hierarchy[i][3] != -1)
		{

			Rect rect = boundingRect(Mat(contours[i]));
			rectangle(image, rect, Scalar(0, 0, 255), 2);

			// ������ľ�����������ľ��εĶԱ�
			if (rect.width < imgCopy.cols * 2 / 7)      //2/7��Ϊ�˷�ֹһЩ΢С�ķ���
				continue;
			if (rect.height < imgCopy.rows * 2 / 7)      //2/7��Ϊ�˷�ֹһЩ΢С�ķ���
				continue;

			// �ж����к�ɫ���ɫ�Ĳ��ֵı���
			if (Rate(dstGray) > 0.20)
			{
				return true;
			}
		}
	}

	return false;
}

double QRLocation::Rate(Mat & count)
{
	int number = 0;
	int allpixel = 0;
	for (int row = 0; row < count.rows; row++)
	{
		for (int col = 0; col < count.cols; col++)
		{
			if (count.at<uchar>(row, col) == 255)
			{
				number++;
			}
			allpixel++;
		}
	}
	//cout << (double)number / allpixel << endl;
	return (double)number / allpixel;
}

Mat QRLocation::transformCorner(Mat src, RotatedRect rect)
{
	//��ȡ��ת����
	Point center = rect.center;
	// ������ϽǺ����½ǵĽǵ㣬����Ҫ��֤������ͼƬ��Χ�����ڿ�ͼ
	Point TopLeft = Point(cvRound(center.x),cvRound(center.y))-Point(rect.size.height / 2, rect.size.width / 2);
	TopLeft.x = TopLeft.x > src.cols ? src.cols : TopLeft.x;
	TopLeft.x = TopLeft.x < 0 ? 0 : TopLeft.x;
	TopLeft.y = TopLeft.y > src.rows ? src.rows : TopLeft.y;
	TopLeft.y = TopLeft.y < 0 ? 0 : TopLeft.y;

	int after_width, after_height;
	if (TopLeft.x + rect.size.width > src.cols) {
		after_width = src.cols - TopLeft.x - 1;
	}
	else {
		after_width = rect.size.width - 1;
	}
	if (TopLeft.y + rect.size.height > src.rows) {
		after_height = src.rows - TopLeft.y - 1;
	}
	else {
		after_height = rect.size.height - 1;
	}

	//��ö�ά���λ��
	Rect RoiRect = Rect(TopLeft.x, TopLeft.y, after_width, after_height);

	//dst�Ǳ���ת��ͼƬ roiΪ���ͼƬ maskΪ��ģ
	double angle = rect.angle;
	Mat mask, roi, dst;
	Mat image;

	// �����н�ͼ��������ͼ��
	vector<Point> contour;
	Point2f points[4];
	rect.points(points);
	for (int i = 0; i < 4; i++)
		contour.push_back(points[i]);

	vector<vector<Point>> contours;
	contours.push_back(contour);

	// ���н�ͼ���л�������
	drawContours(mask, contours, 0, Scalar(255, 255, 255), -1);

	// ͨ��mask��Ĥ��src���ض�λ�õ����ؿ�����dst�С�
	src.copyTo(dst, mask);

	Mat M = getRotationMatrix2D(center, angle, 1);
	warpAffine(dst, image, M, src.size());
	// ��ͼ
	roi = image(RoiRect);

	return roi;
}

Mat QRLocation::transformQRcode(Mat src, RotatedRect rect, double angle)
{
	// �����ת����
	Point center = rect.center;
	// ������ϽǺ����½ǵĽǵ㣬����Ҫ��֤������ͼƬ��Χ�����ڿ�ͼ
	Point TopLeft = Point(cvRound(center.x), cvRound(center.y)) - Point(rect.size.height / 2, rect.size.width / 2);  //��ת���Ŀ��λ��
	TopLeft.x = TopLeft.x > src.cols ? src.cols : TopLeft.x;
	TopLeft.x = TopLeft.x < 0 ? 0 : TopLeft.x;
	TopLeft.y = TopLeft.y > src.rows ? src.rows : TopLeft.y;
	TopLeft.y = TopLeft.y < 0 ? 0 : TopLeft.y;

	int after_width, after_height;
	if (TopLeft.x + rect.size.width > src.cols) {
		after_width = src.cols - TopLeft.x - 1;
	}
	else {
		after_width = rect.size.width - 1;
	}
	if (TopLeft.y + rect.size.height > src.rows) {
		after_height = src.rows - TopLeft.y - 1;
	}
	else {
		after_height = rect.size.height - 1;
	}
	// ��ö�ά���λ��
	Rect RoiRect = Rect(TopLeft.x, TopLeft.y, after_width, after_height);

	// dst�Ǳ���ת��ͼƬ��roiΪ���ͼƬ��maskΪ��ģ
	Mat mask, roi, dst;
	Mat image;
	// �����н�ͼ��������ͼ��

	vector<Point> contour;
	// ��þ��ε��ĸ���
	Point2f points[4];
	rect.points(points);
	for (int i = 0; i < 4; i++)
		contour.push_back(points[i]);

	vector<vector<Point>> contours;
	contours.push_back(contour);
	// ���н�ͼ���л�������
	drawContours(mask, contours, 0, Scalar(255, 255, 255), -1);
	// ͨ��mask��Ĥ��src���ض�λ�õ����ؿ�����dst�С�
	src.copyTo(dst, mask);
	// ��ת
	Mat M = getRotationMatrix2D(center, angle, 1);
	warpAffine(dst, image, M, src.size());
	// ��ͼ
	roi = image(RoiRect);

	return roi;
}

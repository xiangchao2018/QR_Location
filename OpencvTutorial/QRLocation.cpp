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
	/*contours是第一次寻找轮廓*/
	/*contours2是筛选出的轮廓*/
	vector<vector<cv::Point>> contours;
	//	用于轮廓检测
	vector<Vec4i> hierarchy;

	findContours(m_srcGray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	//记录小方块的数量
	int numOfRects = 0;
	//检测方块
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
				//画图部分
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

	//连接三个正方形
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
	// 在原图中画出二维码的区域
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
	// 计算“回”的次序关系
	if (m_centers.size()!=3)
	{
		cout << m_centers.size()<<endl;
		cout << "not find QRcode..."<<endl;
		return;
	}
	int leftTopPointIndex = LeftTopPoint(m_centers);
	vector<int> otherTwoPointIndex = OtherTwoPoint(m_centers, leftTopPointIndex);
	// canvas上标注三个“回”的次序关系
	circle(m_canvas, m_centers[leftTopPointIndex], 10, Scalar(255, 0, 255), -1);
	circle(m_canvas, m_centers[otherTwoPointIndex[0]], 10, Scalar(0, 255, 0), -1);
	circle(m_canvas, m_centers[otherTwoPointIndex[1]], 10, Scalar(0, 255, 255), -1);
	// 计算旋转角
	double angle = rotateAngle(m_centers[leftTopPointIndex], m_centers[otherTwoPointIndex[0]], m_centers[otherTwoPointIndex[1]]);

	// 拿出之前得到的最大的轮廓
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
	double angle = atan(k) * 180 / CV_PI;//转化角度
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
	//定义mask
	Mat imgCopy, dstCopy;
	Mat dstGray;
	imgCopy = image.clone();

	// 转化为灰度图像
	cvtColor(image, dstGray, COLOR_BGR2GRAY);
	// 进行二值化
	threshold(dstGray, dstGray, 0, 255, THRESH_BINARY | THRESH_OTSU);

	dstCopy = dstGray.clone();  //备份

	Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(dstCopy, dstCopy, MORPH_OPEN, erodeStruct);
	// 找到轮廓与传递关系
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

			// 最里面的矩形与最外面的矩形的对比
			if (rect.width < imgCopy.cols * 2 / 7)      //2/7是为了防止一些微小的仿射
				continue;
			if (rect.height < imgCopy.rows * 2 / 7)      //2/7是为了防止一些微小的仿射
				continue;

			// 判断其中黑色与白色的部分的比例
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
	//获取旋转中心
	Point center = rect.center;
	// 获得左上角和右下角的角点，而且要保证不超出图片范围，用于抠图
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

	//获得二维码的位置
	Rect RoiRect = Rect(TopLeft.x, TopLeft.y, after_width, after_height);

	//dst是被旋转的图片 roi为输出图片 mask为掩模
	double angle = rect.angle;
	Mat mask, roi, dst;
	Mat image;

	// 建立中介图像辅助处理图像
	vector<Point> contour;
	Point2f points[4];
	rect.points(points);
	for (int i = 0; i < 4; i++)
		contour.push_back(points[i]);

	vector<vector<Point>> contours;
	contours.push_back(contour);

	// 再中介图像中画出轮廓
	drawContours(mask, contours, 0, Scalar(255, 255, 255), -1);

	// 通过mask掩膜将src中特定位置的像素拷贝到dst中。
	src.copyTo(dst, mask);

	Mat M = getRotationMatrix2D(center, angle, 1);
	warpAffine(dst, image, M, src.size());
	// 截图
	roi = image(RoiRect);

	return roi;
}

Mat QRLocation::transformQRcode(Mat src, RotatedRect rect, double angle)
{
	// 获得旋转中心
	Point center = rect.center;
	// 获得左上角和右下角的角点，而且要保证不超出图片范围，用于抠图
	Point TopLeft = Point(cvRound(center.x), cvRound(center.y)) - Point(rect.size.height / 2, rect.size.width / 2);  //旋转后的目标位置
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
	// 获得二维码的位置
	Rect RoiRect = Rect(TopLeft.x, TopLeft.y, after_width, after_height);

	// dst是被旋转的图片，roi为输出图片，mask为掩模
	Mat mask, roi, dst;
	Mat image;
	// 建立中介图像辅助处理图像

	vector<Point> contour;
	// 获得矩形的四个点
	Point2f points[4];
	rect.points(points);
	for (int i = 0; i < 4; i++)
		contour.push_back(points[i]);

	vector<vector<Point>> contours;
	contours.push_back(contour);
	// 再中介图像中画出轮廓
	drawContours(mask, contours, 0, Scalar(255, 255, 255), -1);
	// 通过mask掩膜将src中特定位置的像素拷贝到dst中。
	src.copyTo(dst, mask);
	// 旋转
	Mat M = getRotationMatrix2D(center, angle, 1);
	warpAffine(dst, image, M, src.size());
	// 截图
	roi = image(RoiRect);

	return roi;
}

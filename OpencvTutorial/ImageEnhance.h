#ifndef _IMAGE_ENHANCE_H_
#define _IMAGE_ENHANCE_H_

#include<iostream>
#include<vector>
#include<string>
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include"opencv2\imgproc\imgproc.hpp"
using namespace std;
using namespace cv;

class ImageEnhance
{
public:
	ImageEnhance();
	~ImageEnhance();
	void TestImageEnhance(string filePath);
};

#endif // _IMAGE_ENHANCE_H_




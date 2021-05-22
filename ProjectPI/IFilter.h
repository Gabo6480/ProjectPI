#pragma once

#include "opencv2/opencv.hpp"

class IFilter
{
public:
	virtual void Filter(cv::Mat& src, cv::Mat& dst) = 0;
};


#include "FilterNegative.h"

void FilterNegative::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::bitwise_not(src, dst);
}
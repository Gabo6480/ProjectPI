#include "FilterDirectionalNorth.h"

FilterDirectionalNorth::FilterDirectionalNorth() {
	float kdata[] = { 1, 1, 1, 1, -2, 1, -1, -1, -1 };
	_kernel = cv::Mat(3, 3, CV_32F, kdata).clone();
}

void FilterDirectionalNorth::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::filter2D(src, dst, -1, _kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
}
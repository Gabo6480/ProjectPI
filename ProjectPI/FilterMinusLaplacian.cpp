#include "FilterMinusLaplacian.h"


FilterMinusLaplacian::FilterMinusLaplacian() {
	float kdata[] = { 0, -1, 0, -1, 5, -1, 0, -1, 0 };
	_kernel = cv::Mat(3, 3, CV_32F, kdata).clone();
}

void FilterMinusLaplacian::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::filter2D(src, dst, -1, _kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
}
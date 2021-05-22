#include "FilterSepia.h"
FilterSepia::FilterSepia() {
	float kdata[] = { 0.272, 0.534, 0.131, 0.349, 0.686, 0.168, 0.393, 0.769, 0.189 };
	_kernel = cv::Mat(3, 3, CV_32F, kdata).clone();
}

void FilterSepia::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::transform(src, dst, _kernel);
}
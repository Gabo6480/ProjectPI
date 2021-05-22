#include "FilterGrayscale.h"

void FilterGrayscale::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);
}
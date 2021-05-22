#include "FilterAverage.h"

void FilterAverage::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::blur(src,dst, _ksize);
}
#include "FilterMedian.h"


FilterMedian::FilterMedian(int ksize) {
	_ksize = ksize % 2 ? ksize : ksize + 1;
	_ksize = std::max(_ksize, 3);
}

void FilterMedian::setValue(float ksize) {
	_ksize = (int)ksize % 2 ? ksize : ksize + 1;
	_ksize = std::max(_ksize, 3);
}

float FilterMedian::getValue() {
	return _ksize;
}

void FilterMedian::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::medianBlur(src, dst, _ksize);
}
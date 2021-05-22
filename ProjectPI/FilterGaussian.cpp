#include "FilterGaussian.h"

FilterGaussian::FilterGaussian(float sigma, cv::Size ksize) {
	_ksize = ksize;
	_sigma = sigma;
}

void FilterGaussian::setValue(float sigma) {
	int v = (int)sigma % 2 ? sigma : sigma + 1;
	v = std::max(v, 3);
	_ksize = cv::Size(v, v);
}

float FilterGaussian::getValue() {
	return _ksize.width;
}

void FilterGaussian::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::GaussianBlur(src, dst, _ksize, _sigma);
}
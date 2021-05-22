#include "FilterThreshold.h"

FilterThreshold::FilterThreshold(double thresh, double maxval) {
	_thresh = thresh;
	_maxval = maxval;
}

void FilterThreshold::setTwoValues(float val, bool isThresh) {
	if(isThresh)
	_thresh = val;
	else
	_maxval = val;
}

float FilterThreshold::getTwoValues(bool isThresh) {
	return isThresh ? _thresh : _maxval;
}

void FilterThreshold::Filter(cv::Mat& src, cv::Mat& dst) {
	threshold(src, dst, _thresh, _maxval, cv::THRESH_BINARY);
}
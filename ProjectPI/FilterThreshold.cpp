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
	const int channels = src.channels();

	int nRows = src.rows;
	int nCols = src.cols;
	int nColsHalve = nCols / 2;
	cv::Mat res = cv::Mat(src.size(), src.type());
	const uchar* srcData = src.data;
	uchar* resData = res.data;

	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nCols; j++) {
			int curr = i * nCols + j;

			for (int k = 0; k < channels; k++) {
				int idx = k + curr * channels;
				resData[idx] = srcData[idx] > _thresh ? _maxval : 0;
			}
		}
	}

	dst = res;
}
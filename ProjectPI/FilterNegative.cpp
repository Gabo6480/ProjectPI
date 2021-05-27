#include "FilterNegative.h"

void FilterNegative::Filter(cv::Mat& src, cv::Mat& dst) {
	const int channels = src.channels();
	int nRows = src.rows;
	int nCols = src.cols;
	cv::Mat res = cv::Mat(src.size(), src.type());
	const uchar* srcData = src.data;
	uchar* resData = res.data;

	for (int i = 0; i < nRows * nCols * channels; i++) {
		resData[i] = ~srcData[i];
	}
	dst = res;
}
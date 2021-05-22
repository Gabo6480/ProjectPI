#include "FilterFourths.h"

void FilterFourths::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::Mat aux;
	int width = src.cols / 2, height = src.rows / 2;
	cv::resize(src, aux, cv::Size(width, height));

	dst = cv::Mat::zeros(cv::Size(src.cols, src.rows), src.type());

	aux.copyTo(dst(cv::Rect(0, 0, width, height)));
	aux.copyTo(dst(cv::Rect(width, 0, width, height)));
	aux.copyTo(dst(cv::Rect(0, height, width, height)));
	aux.copyTo(dst(cv::Rect(width, height, width, height)));
}
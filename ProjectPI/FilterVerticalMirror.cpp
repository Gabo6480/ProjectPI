#include "FilterVerticalMirror.h"

void FilterVerticalMirror::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::Mat aux;
	cv::flip(src, aux, 1);

	aux = aux(cv::Rect(0, 0, aux.cols / 2, aux.rows));

	aux.copyTo(src(cv::Rect(0, 0, aux.cols, aux.rows)));

	dst = src;
}

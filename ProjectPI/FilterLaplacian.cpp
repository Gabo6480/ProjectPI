#include "FilterLaplacian.h"

void FilterLaplacian::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::Laplacian(src, dst, src.depth());
}
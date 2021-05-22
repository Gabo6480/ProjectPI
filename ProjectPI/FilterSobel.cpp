#include "FilterSobel.h"

void FilterSobel::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::Mat auxh, auxw;
	cv::Sobel(src, auxh, -1, 1, 0);
	cv::Sobel(src, auxw, -1, 0, 1);

	cv::addWeighted(auxh, 0.5, auxw, 0.5, 0, dst);
}
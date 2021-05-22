#include "FilterWeightedAverage.h"


FilterWeightedAverage::FilterWeightedAverage(float weight) {
	_weight = weight;
	float kdata[] = { 1, 1, 1, 1, weight, 1, 1, 1, 1 };
	_kernel = cv::Mat(3,3, CV_32F, kdata).clone() / (float) (8 + weight);
}

void FilterWeightedAverage::setValue(float weight) {
	_weight = weight;
	float kdata[] = { 1, 1, 1, 1, weight, 1, 1, 1, 1 };
	_kernel = cv::Mat(3, 3, CV_32F, kdata).clone() / (float)(8 + weight);
}

float FilterWeightedAverage::getValue() {
	return _weight;
}

void FilterWeightedAverage::Filter(cv::Mat& src, cv::Mat& dst) {
	cv::filter2D(src, dst, -1, _kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
}
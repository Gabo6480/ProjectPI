#include "FilterPowerEqualization.h"

FilterPowerEqualization::FilterPowerEqualization(float c, float y) {
	_c = c;
	_y = y;
}

void FilterPowerEqualization::setTwoValues(float val, bool isC) {
	if(isC)
	_c = val;
	else
	_y = val;
}

float FilterPowerEqualization::getTwoValues(bool isC) {
	return isC ? _c : _y;
}

void FilterPowerEqualization::Filter(cv::Mat& src, cv::Mat& dst) {
	std::vector<cv::Mat> planes;
	cv::split(src, planes);


	int histSize = 256;

	float range[] = { 0, 256 }; //the upper boundary is exclusive
	const float* histRange = { range };

	bool uniform = true, accumulate = false;

	std::vector<cv::Mat> hists;
	for (auto& plane : planes) {
		hists.push_back(cv::Mat());

		cv::calcHist(&plane, 1, 0, cv::Mat(), hists.back(), 1, &histSize, &histRange, uniform, accumulate);
	}

	std::vector<uchar*> sums;
	for (auto& hist : hists) {
		uchar* sum = new uchar[256]{ 0 };

		for (int i = 0; i < hist.rows * hist.cols; i++) {
			float p = hist.at<float>(i);
			p = pow(p, _y) * _c;
			sum[i] = cv::saturate_cast<uchar>(p);
		}

		sums.push_back(sum);
	}

	int i = 0;
	for (auto& plane : planes) {
		for (int j = 0; j < plane.rows * plane.cols; j++) {
			uchar p = plane.at<uchar>(j);
			plane.at<uchar>(j) = sums[i][(int)p];
		}
		i++;
	}

	for (auto& sum : sums) {
		delete sum;
	}
	sums.clear();

	cv::merge(planes, dst);
}
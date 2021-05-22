#include "FilterUniformEqualization.h"

void FilterUniformEqualization::Filter(cv::Mat& src, cv::Mat& dst) {
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

		float sumaux[256] = { 0 };
		uchar* sum = new uchar[256]{ 0 };

		sumaux[0] = hist.at<float>(0);

		float min = -1, max = 0;
		for (int i = 1; i < hist.rows * hist.cols; i++) {
			int b = i - 1;
			float histVal = hist.at<float>(b);
			sumaux[i] = sumaux[b] + hist.at<float>(i);

			if (histVal == 0)
				continue;

			if (min == -1)
				min = b;

			max = b;
		}

		float sumDiff = max - min;
		float summax = sumaux[255];

		for (int i = 0; i < hist.rows * hist.cols; i++) {
			float p = sumaux[i];
			p /= summax;
			p *= sumDiff;
			sum[i] = p + min;
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
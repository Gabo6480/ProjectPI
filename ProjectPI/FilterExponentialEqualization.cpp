#include "FilterExponentialEqualization.h"

FilterExponentialEqualization::FilterExponentialEqualization(float a) {
	_a = a;
}

void FilterExponentialEqualization::setValue(float a) {
	_a = a;
}

float FilterExponentialEqualization::getValue() {
	return _a;
}

void FilterExponentialEqualization::Filter(cv::Mat& src, cv::Mat& dst) {
	const int channels = src.channels();
	const int histSize = 256;

	int nRows = src.rows;
	int nCols = src.cols;
	cv::Mat res = cv::Mat(src.size(), src.type());
	const uchar* srcData = src.data;
	uchar* resData = res.data;

	std::vector<int*> hists;
	for (int i = 0; i < channels; i++) {
		int* hist = new int[histSize] { 0 };
		hists.push_back(hist);
	}

	for (int i = 0; i < channels; i++) {
		int* hist = hists[i];
		for (int j = 0; j < nRows * nCols; j++) {
			hist[(int)srcData[i + j * channels]]++;
		}
	}

	std::vector<uchar*> sums;
	for (auto& hist : hists) {

		float sumaux[256] = { 0 };
		uchar* sum = new uchar[256]{ 0 };

		sumaux[0] = hist[0];

		float min = -1;
		for (int i = 1; i < histSize; i++) {
			int b = i - 1;
			float histVal = hist[b];
			sumaux[i] = sumaux[b] + hist[i];

			if (histVal == 0)
				continue;

			if (min == -1)
				min = b;
		}
		float sumDelta = 1.f / _a;
		float summax = sumaux[255];

		for (int i = 0; i < histSize; i++) {
			float p = sumaux[i];
			p /= summax; //CDF
			p = p < 1 ? log(1 - p) : log(0.001);
			p *= sumDelta;
			sum[i] = cv::saturate_cast<uchar>(min - p);
		}

		sums.push_back(sum);
	}

	for (int i = 0; i < channels; i++) {
		for (int j = 0; j < nRows * nCols; j++) {
			int idx = i + j * channels;
			resData[idx] = sums[i][(int)srcData[idx]];
		}
	}

	for (int i = 0; i < channels; i++) {
		delete[] sums[i];
		delete[] hists[i];
	}
	sums.clear();
	hists.clear();

	dst = res;
}
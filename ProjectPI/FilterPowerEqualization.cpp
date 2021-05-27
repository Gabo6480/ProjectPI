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
		uchar* sum = new uchar[256]{ 0 };

		for (int i = 0; i < histSize; i++) {
			float p = hist[i];
			p = pow(p, _y) * _c;
			sum[i] = cv::saturate_cast<uchar>(p);
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
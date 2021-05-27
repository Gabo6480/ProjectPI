#include "FilterUniformEqualization.h"

void FilterUniformEqualization::Filter(cv::Mat& src, cv::Mat& dst) {
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
	for (int i = 0; i < channels; i++) {

		float sumaux[256] = { 0 };
		uchar* sum = new uchar[256]{ 0 };

		sumaux[0] = hists[i][0];

		float min = -1, max = 0;
		for (int j = 1; j < histSize; j++) {
			int b = j - 1;
			float histVal = hists[i][b];
			sumaux[j] = sumaux[b] + hists[i][j];

			if (histVal == 0)
				continue;

			if (min == -1)
				min = b;

			max = b;
		}

		float sumDiff = max - min;
		float summax = sumaux[255];

		for (int j = 0; j < histSize; j++) {
			float p = sumaux[j];
			p /= summax;
			p *= sumDiff;
			sum[j] = p + min;
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
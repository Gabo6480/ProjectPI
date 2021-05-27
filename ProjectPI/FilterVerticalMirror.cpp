#include "FilterVerticalMirror.h"

void FilterVerticalMirror::Filter(cv::Mat& src, cv::Mat& dst) {
	const int channels = src.channels();

	int nRows = src.rows;
	int nCols = src.cols;
	int nColsHalve = nCols / 2;
	cv::Mat res = cv::Mat(src.size(), src.type());
	const uchar* srcData = src.data;
	uchar* resData = res.data;

	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nCols; j++) {
			int curr = i * nCols + j;
			int target = j < nColsHalve ? curr : i * nCols + nColsHalve + (nColsHalve - j);

			for (int k = 0; k < channels; k++) {
				int idx = k + curr * channels;
				int idxT = k + target * channels;
				resData[idx] = srcData[idxT];
			}
		}
	}

	dst = res;
}

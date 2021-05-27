#include "FilterFourths.h"

void FilterFourths::Filter(cv::Mat& src, cv::Mat& dst) {
	const int channels = src.channels();

	int nRows = src.rows;
	int nCols = src.cols;

	cv::Mat aux;
	int width = nCols / 2, height = nRows / 2;
	cv::resize(src, aux, cv::Size(width, height));

	cv::Mat res = cv::Mat(src.size(), src.type());
	const uchar* srcData = aux.data;
	uchar* resData = res.data;

	int sRows = aux.rows;
	int sCols = aux.cols;

    for (int i = 0; i < sRows; i++) {
		memcpy_s(&resData[(i * nCols) * channels], sCols * channels, &srcData[(i * sCols) * channels], sCols * channels);

		memcpy_s(&resData[(i * nCols + sCols) * channels], sCols * channels, &srcData[(i * sCols) * channels], sCols * channels);

		memcpy_s(&resData[(i * nCols + sRows * nCols) * channels], sCols * channels, &srcData[(i * sCols) * channels], sCols * channels);

		memcpy_s(&resData[(i * nCols + sCols + sRows * nCols) * channels], sCols * channels, &srcData[(i * sCols) * channels], sCols * channels);
    }

	dst = res;
}
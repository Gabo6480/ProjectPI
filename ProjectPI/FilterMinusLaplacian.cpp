#include "FilterMinusLaplacian.h"
//
//
//FilterMinusLaplacian::FilterMinusLaplacian() {
//	float kdata[] = { 0, -1, 0, -1, 5, -1, 0, -1, 0 };
//	_kernel = cv::Mat(3, 3, CV_32F, kdata).clone();
//}

void FilterMinusLaplacian::Filter(cv::Mat& src, cv::Mat& dst) {
    const int channels = src.channels();

    int nRows = src.rows;
    int nCols = src.cols;

    cv::Mat res = cv::Mat(src.size(), src.type());
    const uchar* srcData = src.data;
    uchar* resData = res.data;

    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) {
            if (i < 1 || i > nRows - 1 || j < 1 || j > nCols - 1)
                memcpy_s(&resData[(j + i * nCols) * channels], channels, &srcData[(j + i * nCols) * channels], channels);
            else
                for (int k = 0; k < channels; k++) {
                    float avr = 0.f;

                    avr += srcData[(j + (i - 1) * nCols) * channels + k] * -1;
                    avr += srcData[((j - 1) + i * nCols) * channels + k] * -1;
                    avr += srcData[(j + i * nCols) * channels + k] * 5;
                    avr += srcData[((j + 1) + i * nCols) * channels + k] * -1;
                    avr += srcData[(j + (i + 1) * nCols) * channels + k]* -1;

                    resData[(j + i * nCols) * channels + k] = std::min(std::max(avr, 0.f), 255.f);
                }
        }
    }

    dst = res;
}
#include "FilterGrayscale.h"

void FilterGrayscale::Filter(cv::Mat& src, cv::Mat& dst) {
    const int channels = src.channels();

    if (channels != 3) {
        dst = src;
        return;
    }

    int nRows = src.rows;
    int nCols = src.cols;

    cv::Mat res = cv::Mat(src.size(), CV_8UC1);
    const uchar* srcData = src.data;
    uchar* resData = res.data;

    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) {
            float res = 0.f;
            res += srcData[(j + i * nCols) * channels + 0] * .3f;
            res += srcData[(j + i * nCols) * channels + 1] * .59f;
            res += srcData[(j + i * nCols) * channels + 2] * .11f;
            resData[(j + i * nCols)] = res;
        }
    }

    dst = res;
}
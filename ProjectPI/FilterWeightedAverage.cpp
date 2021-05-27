#include "FilterWeightedAverage.h"


FilterWeightedAverage::FilterWeightedAverage(float weight) {
	_weight = weight;
}

void FilterWeightedAverage::setValue(float weight) {
	_weight = weight;
}

float FilterWeightedAverage::getValue() {
	return _weight;
}

void FilterWeightedAverage::Filter(cv::Mat& src, cv::Mat& dst) {
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

                    avr += srcData[((j - 1) + (i - 1) * nCols) * channels + k];
                    avr += srcData[(j + (i - 1) * nCols) * channels + k];
                    avr += srcData[((j + 1) + (i - 1) * nCols) * channels + k];
                    avr += srcData[((j - 1) + i * nCols) * channels + k];
                    avr += srcData[(j + i * nCols) * channels + k] * _weight;
                    avr += srcData[((j + 1) + i * nCols) * channels + k];
                    avr += srcData[((j - 1) + (i + 1) * nCols) * channels + k];
                    avr += srcData[(j + (i + 1) * nCols) * channels + k];
                    avr += srcData[((j + 1) + (i + 1) * nCols) * channels + k];

                    avr /= 8.f + _weight;
                    resData[(j + i * nCols) * channels + k] = avr;
                }
        }
    }

    dst = res;
}
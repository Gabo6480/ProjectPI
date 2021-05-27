#include "FilterGaussian.h"
# define M_PI           3.14159265358979323846

void FilterGaussian::calcMatrix() {
    _ksize = 5;

    for (int i = 0; i < _ksize; i++) {
        for (int j = 0; j < _ksize; j++) {
            int val = std::exp(-(std::pow(i, 2) + std::pow(j, 2)) / std::pow(_sigma, 2)) / 2 * M_PI * std::pow(_sigma, 2);
            sum += val;
            matrix[j + i * _ksize] = val;
        }
    }
}
FilterGaussian::~FilterGaussian() {
    delete[] matrix;
}

FilterGaussian::FilterGaussian(float sigma) {
	_sigma = sigma;
    matrix = new int[5 * 5] { 0 };
    calcMatrix();
}

void FilterGaussian::setValue(float sigma) {
    _sigma = sigma;
    calcMatrix();
}

float FilterGaussian::getValue() {
	return _sigma;
}

void FilterGaussian::Filter(cv::Mat& src, cv::Mat& dst) {
    const int channels = src.channels();

    int nRows = src.rows;
    int nCols = src.cols;
    int halve = std::trunc(_ksize/ 2.0);

    cv::Mat res = cv::Mat(src.size(), src.type());
    const uchar* srcData = src.data;
    uchar* resData = res.data;

    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) {
            if (i < halve || i > nRows - halve || j < halve || j > nCols - halve)
                memcpy_s(&resData[(j + i * nCols) * channels], channels, &srcData[(j + i * nCols) * channels], channels);
            else
                for (int k = 0; k < channels; k++) {
                    float avr = 0.f;
                    for (int x = 0; x < _ksize; x++) {
                        for (int y = 0; y < _ksize; y++) {
                            avr += srcData[(((j - halve) + x) + ((i - halve) + y) * nCols) * channels + k] * matrix[x + y * _ksize];
                        }
                    }

                    avr /= (float)sum;
                    resData[(j + i * nCols) * channels + k] = avr;
                }
        }
    }

    dst = res;
}
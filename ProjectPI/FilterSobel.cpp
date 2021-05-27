#include "FilterSobel.h"

void FilterSobel::Filter(cv::Mat& src, cv::Mat& dst) {
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
                    float avr1 = 0.f;
                    float avr2 = 0.f;

                    avr1 += srcData[((j - 1) + (i - 1) * nCols) * channels + k] * -1;
                    //avr1 += srcData[(j + (i - 1) * nCols) * channels + k];
                    avr1 += srcData[((j + 1) + (i - 1) * nCols) * channels + k];
                    avr1 += srcData[((j - 1) + i * nCols) * channels + k] * -2;
                    //avr1 += srcData[(j + i * nCols) * channels + k];
                    avr1 += srcData[((j + 1) + i * nCols) * channels + k] * 2;
                    avr1 += srcData[((j - 1) + (i + 1) * nCols) * channels + k] * -1;
                    //avr1 += srcData[(j + (i + 1) * nCols) * channels + k];
                    avr1 += srcData[((j + 1) + (i + 1) * nCols) * channels + k];

                    avr2 += srcData[((j - 1) + (i - 1) * nCols) * channels + k] * -1;
                    avr2 += srcData[(j + (i - 1) * nCols) * channels + k] * -2;
                    avr2 += srcData[((j + 1) + (i - 1) * nCols) * channels + k] * -1;
                    //avr2 += srcData[((j - 1) + i * nCols) * channels + k];
                    //avr2 += srcData[(j + i * nCols) * channels + k];
                    //avr2 += srcData[((j + 1) + i * nCols) * channels + k];
                    avr2 += srcData[((j - 1) + (i + 1) * nCols) * channels + k];
                    avr2 += srcData[(j + (i + 1) * nCols) * channels + k] * 2;
                    avr2 += srcData[((j + 1) + (i + 1) * nCols) * channels + k];

                    resData[(j + i * nCols) * channels + k] = std::min(std::sqrtf(std::pow(avr1, 2) + std::pow(avr2, 2)), 255.f);
                }
        }
    }

    dst = res;
}
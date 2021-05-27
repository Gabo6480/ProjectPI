#include "FilterSepia.h"

void FilterSepia::Filter(cv::Mat& src, cv::Mat& dst) {
    const int channels = src.channels();

    if (channels != 3) {
        dst = src;
        return;
    }

    int nRows = src.rows;
    int nCols = src.cols;

    cv::Mat res = cv::Mat(src.size(), src.type());
    const uchar* srcData = src.data;
    uchar* resData = res.data;

    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) {
            int curr = (j + i * nCols) * channels;
            float b = srcData[curr + 0];
            float g = srcData[curr + 1];
            float r = srcData[curr + 2];
            resData[curr + 0] = std::min(std::max(r * .272 + g * .534 + b * .131, 0.0), 255.0);
            resData[curr + 1] = std::min(std::max(r * .349 + g * .686 + b * .168, 0.0), 255.0);
            resData[curr + 2] = std::min(std::max(r * .393 + g * .769 + b * .189, 0.0), 255.0);
        }
    }

    dst = res;
}
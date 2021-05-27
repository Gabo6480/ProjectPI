#include "FilterMedian.h"


FilterMedian::FilterMedian(int ksize) {
	_ksize = ksize % 2 ? ksize : ksize + 1;
	_ksize = std::max(_ksize, 3);
}

void FilterMedian::setValue(float ksize) {
	_ksize = (int)ksize % 2 ? ksize : ksize + 1;
	_ksize = std::max(_ksize, 3);
}

float FilterMedian::getValue() {
	return _ksize;
}

void FilterMedian::Filter(cv::Mat& src, cv::Mat& dst) {
    const int channels = src.channels();

    int nRows = src.rows;
    int nCols = src.cols;
    int halve = (_ksize - 1) / 2;

    cv::Mat res = cv::Mat(src.size(), src.type());
    const uchar* srcData = src.data;
    uchar* resData = res.data;

    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) {
            if (i < 1 || i > nRows - 1 || j < 1 || j > nCols - 1)
                memcpy_s(&resData[(j + i * nCols) * channels], channels, &srcData[(j + i * nCols) * channels], channels);
            else {
                std::multiset<int> nums;
                for (int k = 0; k < channels; k++) {
                    
                    for (int x = 0; x < _ksize; x++) {
                        for (int y = 0; y < _ksize; y++) {
                            nums.insert(srcData[(((j - halve) + x) + ((i - halve) + y) * nCols) * channels + k]);
                        }
                    }

                    resData[(j + i * nCols) * channels + k] = *std::next(nums.begin(), nums.size() / 2);
                    nums.clear();
                }
            }
        }
    }

    dst = res;
}
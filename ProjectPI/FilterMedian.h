#pragma once
#include "IFilter.h"
#include "ISetValue.h"
class FilterMedian :
    public IFilter, public ISetValue
{
    int _ksize = 3;
public:
    FilterMedian(int ksize = 3);

    void Filter(cv::Mat& src, cv::Mat& dst);

    void setValue(float ksize);
    float getValue();
};


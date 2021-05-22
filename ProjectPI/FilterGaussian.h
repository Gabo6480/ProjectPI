#pragma once
#include "IFilter.h"
#include "ISetValue.h"
class FilterGaussian :
    public IFilter, public ISetValue
{
    cv::Size _ksize;
    float _sigma = 0;
public:

    FilterGaussian(float sigma = 0, cv::Size ksize = cv::Size(3, 3));

    void Filter(cv::Mat& src, cv::Mat& dst);

    void setValue(float sigma);
    float getValue();
};


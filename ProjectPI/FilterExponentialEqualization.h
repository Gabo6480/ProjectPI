#pragma once
#include "IFilter.h"
#include "ISetValue.h"
class FilterExponentialEqualization :
    public IFilter, public ISetValue
{
    float _a;
public:
    FilterExponentialEqualization(float a = 0.1f);

    void Filter(cv::Mat& src, cv::Mat& dst);

    void setValue(float a);
    float getValue();
};


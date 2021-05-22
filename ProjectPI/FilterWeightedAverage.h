#pragma once
#include "IFilter.h"
#include "ISetValue.h"
class FilterWeightedAverage :
    public IFilter, public ISetValue
{
    float _weight;
    cv::Mat _kernel;
public:
    FilterWeightedAverage(float weight = 2.0f);

    void Filter(cv::Mat& src, cv::Mat& dst);

    void setValue(float weight);

    float getValue();
};


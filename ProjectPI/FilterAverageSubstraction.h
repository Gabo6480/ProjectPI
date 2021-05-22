#pragma once
#include "IFilter.h"
class FilterAverageSubstraction :
    public IFilter
{
    cv::Mat _kernel;
public:
    FilterAverageSubstraction();

    void Filter(cv::Mat& src, cv::Mat& dst);
};


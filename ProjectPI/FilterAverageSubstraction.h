#pragma once
#include "IFilter.h"
class FilterAverageSubstraction :
    public IFilter
{
public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


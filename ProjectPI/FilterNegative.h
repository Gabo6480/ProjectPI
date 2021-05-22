#pragma once
#include "IFilter.h"
class FilterNegative :
    public IFilter
{
public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


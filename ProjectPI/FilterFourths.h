#pragma once
#include "IFilter.h"
class FilterFourths :
    public IFilter
{
public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


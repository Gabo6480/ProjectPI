#pragma once
#include "IFilter.h"
class FilterLaplacian :
    public IFilter
{
public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


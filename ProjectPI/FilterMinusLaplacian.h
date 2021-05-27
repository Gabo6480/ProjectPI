#pragma once
#include "IFilter.h"
class FilterMinusLaplacian :
    public IFilter
{
public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


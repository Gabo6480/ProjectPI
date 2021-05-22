#pragma once
#include "IFilter.h"
class FilterMinusLaplacian :
    public IFilter
{
    cv::Mat _kernel;
public:
    FilterMinusLaplacian();

    void Filter(cv::Mat& src, cv::Mat& dst);
};


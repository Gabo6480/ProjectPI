#pragma once
#include "IFilter.h"
class FilterDirectionalEast :
    public IFilter
{
    cv::Mat _kernel;
public:
    FilterDirectionalEast();

    void Filter(cv::Mat& src, cv::Mat& dst);
};


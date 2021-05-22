#pragma once
#include "IFilter.h"
class FilterDirectionalNorth :
    public IFilter
{
    cv::Mat _kernel;
public:
    FilterDirectionalNorth();

    void Filter(cv::Mat& src, cv::Mat& dst);
};


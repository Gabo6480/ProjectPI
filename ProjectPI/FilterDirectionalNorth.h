#pragma once
#include "IFilter.h"
class FilterDirectionalNorth :
    public IFilter
{
public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


#pragma once
#include "IFilter.h"
class FilterDirectionalEast :
    public IFilter
{
public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


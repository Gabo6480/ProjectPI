#pragma once
#include "IFilter.h"
class FilterUniformEqualization :
    public IFilter
{
    void Filter(cv::Mat& src, cv::Mat& dst);
};


#pragma once
#include "IFilter.h"
class FilterGrayscale :
    public IFilter
{
public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


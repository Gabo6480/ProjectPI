#pragma once
#include "IFilter.h"
class FilterSepia :
    public IFilter
{
    cv::Mat _kernel;
public:
    FilterSepia();

    void Filter(cv::Mat& src, cv::Mat& dst);
};


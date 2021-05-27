#pragma once
#include "IFilter.h"
class FilterSepia :
    public IFilter
{
public:

    void Filter(cv::Mat& src, cv::Mat& dst);
};


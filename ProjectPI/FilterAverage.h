#pragma once
#include "IFilter.h"
class FilterAverage :
    public IFilter
{
    cv::Size _ksize = cv::Size(3, 3);

public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


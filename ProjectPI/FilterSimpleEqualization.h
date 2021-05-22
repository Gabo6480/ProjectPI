#pragma once
#include "IFilter.h"
class FilterSimpleEqualization :
    public IFilter
{
public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


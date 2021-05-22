#pragma once
#include "IFilter.h"
class FilterSobel :
    public IFilter
{
public:
    void Filter(cv::Mat& src, cv::Mat& dst);
};


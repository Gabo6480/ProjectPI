#pragma once
#include "IFilter.h"
#include "ISetTwoValues.h"
class FilterThreshold :
    public IFilter, public ISetTwoValues
{
    double _thresh, _maxval;
public:
    FilterThreshold(double thresh = 30, double maxval = 255);
    void Filter(cv::Mat& src, cv::Mat& dst);

    void setTwoValues(float val, bool isThresh);
    float getTwoValues(bool isThresh);
};


#pragma once
#include "IFilter.h"
#include "ISetTwoValues.h"
class FilterPowerEqualization :
    public IFilter, public ISetTwoValues
{
    float _c;
    float _y;
public:
    FilterPowerEqualization(float c = 1.f, float y = 1.2f);

    void Filter(cv::Mat& src, cv::Mat& dst);

    void setTwoValues(float val, bool isC);

    float getTwoValues(bool isC);
};


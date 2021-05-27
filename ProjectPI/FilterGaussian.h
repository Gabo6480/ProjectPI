#pragma once
#include "IFilter.h"
#include "ISetValue.h"
class FilterGaussian :
    public IFilter, public ISetValue
{
    float _sigma = 8;
    int* matrix = 0;
    int _ksize = 0;
    int sum = 0;
public:

    FilterGaussian(float sigma = 8);
    ~FilterGaussian();

    void Filter(cv::Mat& src, cv::Mat& dst);

    void calcMatrix();

    void setValue(float sigma);
    float getValue();
};


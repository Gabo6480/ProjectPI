#pragma once
class ISetTwoValues
{
public:
	virtual void setTwoValues(float value, bool isOne) = 0;
	virtual float getTwoValues(bool isOne) = 0;
};


#pragma once
class ISetValue
{
public:
	virtual void setValue(float value) = 0;
	virtual float getValue() = 0;
};


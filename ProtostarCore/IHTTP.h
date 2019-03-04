#pragma once
#include <iostream>
#include <sstream>
#include <string>

class IHTTP
{
public:
	IHTTP();

	virtual ~IHTTP(){}
	virtual void Request() = 0;
	virtual std::stringstream Response() = 0;
};


#pragma once
#include <string>
#include "DataSnap.h"

class CSimpleSaver
{
public:
	CSimpleSaver();
	void SaveVelocityVsNoise(std::string fName, CDataSnap ds);
	~CSimpleSaver();
};


#pragma once
#include <string>
#include "DataSnap.h"

class CSimpleSaver
{
	friend class CDataSnap;
public:
	CSimpleSaver();
	void SaveVelocityVsNoise(std::string fName, CDataSnap ds);
	~CSimpleSaver();
};


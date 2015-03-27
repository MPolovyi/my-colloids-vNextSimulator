#include "stdafx.h"
#include <fstream>

#include "SimpleSaver.h"


CSimpleSaver::CSimpleSaver()
{}

void CSimpleSaver::SaveVelocityVsNoise(std::string fName, CDataSnap ds)
{
	std::ofstream myfile;
	myfile.open(fName);
	myfile << "GUID = " << ds.simGuid;
	for (auto data : ds.AverageVelocityData)
	{
		myfile << ds.Noize << " " << blaze::length(std::get<2>(data)) << " " << std::get<3>(data) << std::endl;
	}
	myfile.close();
}

CSimpleSaver::~CSimpleSaver()
{}

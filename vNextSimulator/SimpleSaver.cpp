#include "stdafx.h"
#include <fstream>

#include "SimpleSaver.h"


CSimpleSaver::CSimpleSaver()
{}

void CSimpleSaver::SaveVelocityVsNoise(std::string fName, CDataSnap ds)
{
	std::ofstream myfile;
	myfile.open(fName, std::ios::app);
	for (auto data : ds.ParticleData)
	{
		myfile << std::get<0>(data)[0] << " " << std::get<0>(data)[1] << " "<< std::get<1>(data)[0] << " " << std::get<1>(data)[1] << std::endl;
	}
	myfile.close();
}

CSimpleSaver::~CSimpleSaver()
{}

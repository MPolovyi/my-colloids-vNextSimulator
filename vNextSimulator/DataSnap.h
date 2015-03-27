#pragma once
#include "Simulator.h"



class CDataSnap
{
public:
	CDataSnap(Simulator::CSimulator& s);
	~CDataSnap();
private:
	double m_Noize;

	int m_stepsFromStart;

	GUID simGuid;

	//Previous noise, Steps with previous noises
	std::vector<std::pair<double, int>> m_PreviousSimulations;

	//Coordinate, Velocity
	std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> m_ParticleData;
	
	//Coordinate, AreaExtents, Velocity
	std::vector<std::tuple<blaze::Vec2d, blaze::Vec2d, blaze::Vec2d>> m_AverageVelocityData;

	//Coordinate, AreaExtents, Dencity
	std::vector<std::tuple<blaze::Vec2d, blaze::Vec2d, double>> m_AverageDencityData;

	void SaveParticleData(Simulator::CSimulator& s);
	void SaveVelocityData(Simulator::CSimulator& s);
	void SaveDencityData(Simulator::CSimulator& s);
	void SaveVelocityData(Simulator::CSimulator& s, std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>>& CoordsAndExtents);
	void SaveDencityData(Simulator::CSimulator& s, std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>>& CoordsAndExtents);

	std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> PrepareSquareExtents(blaze::Vec2d squareArea, int numX, int numY);
};
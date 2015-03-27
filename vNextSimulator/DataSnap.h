#pragma once
#include "Simulator.h"

class CDataSnap
{
	typedef std::tuple<blaze::Vec2d, blaze::Vec2d, blaze::Vec2d, double> Coord_AreaExtent_Velocity_NumParticles;
	typedef std::tuple<blaze::Vec2d, blaze::Vec2d, double, double> Coord_AreaExtent_Dencity_NumParticles;
public:
	CDataSnap(Simulator::CSimulator& s);
	~CDataSnap();

	double Noize;
	int stepsFromStart;
	std::string simGuid;

	//Previous noise, Steps with previous noises
	std::vector<std::pair<double, int>> PreviousSimulations;

	//Coordinate, Velocity
	std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> ParticleData;
	
	//Coordinate, AreaExtents, Velocity, NumParticles
	std::vector<Coord_AreaExtent_Velocity_NumParticles> AverageVelocityData;

	//Coordinate, AreaExtents, Dencity, NumParticles
	std::vector<Coord_AreaExtent_Dencity_NumParticles> AverageDencityData;

	void SaveParticleData(Simulator::CSimulator& s);
	void SaveVelocityData(Simulator::CSimulator& s);
	void SaveDencityData(Simulator::CSimulator& s);
	void SaveVelocityData(Simulator::CSimulator& s, std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>>& CoordsAndExtents);
	void SaveDencityData(Simulator::CSimulator& s, std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>>& CoordsAndExtents);

	std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> PrepareSquareExtents(blaze::Vec2d squareArea, int numX, int numY);
};
#pragma once
#include "stdafx.h"
#include "DataSnap.h"

#include <thread>

CDataSnap::CDataSnap(Simulator::CSimulator& s)
{
	std::cout << "Creating data snap" << std::endl;
	SaveMiscInfo(s);
	SaveParticleData(s);
	SaveVelocityData(s);
	SaveDencityData(s);
}

void CDataSnap::SaveMiscInfo(Simulator::CSimulator& s)
{
	simGuid = s.Guid;
	stepsFromStart = s.Steps;
	Noise = s.GetNoise();
	PreviousSimulations = std::vector<std::pair<double, int>>(s.PreviousNoises.begin(), s.PreviousNoises.end());
	ParticleCount = s.GetParticleCount();
	ParticleVelocity = s.GetParticleVelocity();
}

CDataSnap::~CDataSnap()
{}

void CDataSnap::SaveParticleData(Simulator::CSimulator& s)
{
	auto particleDat = s.GetParticles();
	ParticleData.reserve(particleDat.size());
	for (auto particle : particleDat)
	{
		ParticleData.push_back(std::pair<blaze::Vec2d, blaze::Vec2d>(
			blaze::Vec2d(particle.Coords),
			blaze::Vec2d(particle.Velocity)));
	}
}

//first vector - coordinate, second - square extents from the coordinate
std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> CDataSnap::PrepareSquareExtents(blaze::Vec2d squareArea, int numX, int numY)
{
	std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> extents;

	double dX = squareArea[0] / (2.0 * numX);
	double dY = squareArea[1] / (2.0 * numY);

	blaze::Vec2d ext(dX, dY);

	double yCoord = dY;
	while (yCoord < squareArea[1])
	{
		double xCoord = dX;
		while (xCoord < squareArea[0])
		{
			extents.push_back(std::pair<blaze::Vec2d, blaze::Vec2d>(blaze::Vec2d(yCoord, xCoord), ext));
			xCoord += dX * 2;
		}
		yCoord += dY * 2;
	}

	return extents;
}

void CDataSnap::SaveVelocityData(Simulator::CSimulator& s) 
{
	SaveVelocityData(s, PrepareSquareExtents(s.Extents, 10, 10));
}

void CDataSnap::SaveVelocityData(Simulator::CSimulator& s,
	std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>>& CoordsAndExtents)
{
	std::vector<blaze::Vec2d> result;
	result.resize(CoordsAndExtents.size());

	std::vector<int> particleCount;
	particleCount.resize(CoordsAndExtents.size());

	auto particles = s.GetParticles();

	std::vector<std::thread> workers;
	for (int i = 0; i < CoordsAndExtents.size(); i++)
	{
		workers.push_back(std::thread(
			[](std::vector<blaze::Vec2d>& result,
			std::vector<Simulator::CParticle>& particles,
			std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>>& CoordsAndExtents,
			std::vector<int>& particleCount,
			int index) {
			for (auto particle : particles)
			{
				blaze::Vec2d particleVector = blaze::abs(particle.Coords - CoordsAndExtents[index].first);

				if (particleVector[0] <= CoordsAndExtents[index].second[0] &&
					particleVector[1] <= CoordsAndExtents[index].second[1])
				{
					result[index] += particle.Velocity;
					particleCount[index] += 1;
				}
			}}, std::ref(result), std::ref(particles), std::ref(CoordsAndExtents), std::ref(particleCount), i));
	}
	std::for_each(workers.begin(), workers.end(), [](std::thread &t) { t.join(); });

	for (int i = 0; i < result.size(); i++)
	{
		AverageVelocityData.push_back(
			Coord_AreaExtent_Velocity_NumParticles(
			CoordsAndExtents[i].first, CoordsAndExtents[i].second, result[i] / particleCount[i], particleCount[i]));
	}
}
void CDataSnap::SaveDencityData(Simulator::CSimulator& s) 
{
	SaveDencityData(s, PrepareSquareExtents(s.Extents, 10, 10));
}

void CDataSnap::SaveDencityData(Simulator::CSimulator& s,
	std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>>& CoordsAndExtents)
{
	std::vector<double> result;
	result.resize(CoordsAndExtents.size());

	std::vector<double> squares;
	squares.resize(CoordsAndExtents.size());
	
	double square = s.Extents[0] * s.Extents[1];

	auto particles = s.GetParticles();

	std::vector<std::thread> workers;
	for (int i = 0; i < CoordsAndExtents.size(); i++)
	{
		workers.push_back(std::thread(
			[](std::vector<double>& result,
			std::vector<Simulator::CParticle>& particles,
			std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>>& CoordsAndExtents,
			int index) {
			for (auto particle : particles)
			{
				blaze::Vec2d particleVector = blaze::abs(particle.Coords - CoordsAndExtents[index].first);
				if (particleVector[0] <= CoordsAndExtents[index].second[0] &&
					particleVector[1] <= CoordsAndExtents[index].second[1])
				{
					result[index] += 1;
				}
			}}, std::ref(result), std::ref(particles), std::ref(CoordsAndExtents), i));

			squares[i] = CoordsAndExtents[i].second[0] * CoordsAndExtents[i].second[0];
	}

	std::for_each(workers.begin(), workers.end(), [](std::thread &t) { t.join(); });


	for (int i = 0; i < CoordsAndExtents.size(); i++)
	{
		AverageDencityData.push_back(
			Coord_AreaExtent_Dencity_NumParticles(CoordsAndExtents[i].first, CoordsAndExtents[i].second, result[i] / squares[i], result[i]));		
	}
}
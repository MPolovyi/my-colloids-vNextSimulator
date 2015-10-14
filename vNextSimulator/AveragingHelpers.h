#pragma once
#include <thread>
#include "Particle.h"

namespace AveragingHelpers
{
	inline int NumOfMeaningfulSplits(int ptCount)
	{
		int minParticletCountExpected = 300 > (ptCount * 0.1) ? (ptCount*0.1) : 300;
		auto tmp = round(sqrt(ptCount / minParticletCountExpected));
		return tmp >= 10 ? tmp : 10;
	}

	template<size_t spDim>
	std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>
		PrepareSquareExtents(blaze::StaticVector<double, spDim> squareArea, int num)
	{
			return PrepareExtents(squareArea, blaze::StaticVector<double, spDim>(num));
		};

	inline std::vector<std::pair<blaze::Vec3d, blaze::Vec3d>> 
		PrepareExtents(blaze::Vec3d& squareArea, blaze::Vec3d& num)
	{
		std::vector<std::pair<blaze::Vec3d, blaze::Vec3d>> extents;
		//TODO:
		//double dX = squareArea[0] / (2.0 * numX);
		//double dY = squareArea[1] / (2.0 * numY);

		//blaze::Vec2d ext(dX, dY);

		//double yCoord = dY;
		//while (yCoord < squareArea[1])
		//{
		//	double xCoord = dX;
		//	while (xCoord < squareArea[0])
		//	{
		//		extents.push_back(std::pair<blaze::Vec2d, blaze::Vec2d>(blaze::Vec2d(yCoord, xCoord), ext));
		//		xCoord += dX * 2;
		//	}
		//	yCoord += dY * 2;
		//}

		return extents;
	}
	
	inline std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> 
		PrepareExtents(blaze::Vec2d& squareArea, blaze::Vec2d& num)
	{
		std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> extents;

		double dX = num[0] != 0 ? squareArea[0] / (2.0 * num[0]) : squareArea[0] / 2;
		double dY = num[1] != 0 ? squareArea[1] / (2.0 * num[1]) : squareArea[1] / 2;

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
	};

	template <size_t spDim>
	void GetParticleVelocityAveragedByExtents(
		const std::vector<Simulator::CParticle<spDim>>& particles,
		std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents,
		std::vector<blaze::StaticVector<double, spDim>>& result,
		std::vector<int>& particleCount)
	{
		result.resize(CoordsAndExtents.size());

		particleCount.resize(CoordsAndExtents.size());
				
		std::vector<std::thread> workers;
		for (int i = 0; i < CoordsAndExtents.size(); i++)
		{
			workers.push_back(std::thread(
				[](std::vector<blaze::StaticVector<double, spDim>>& result,
				std::vector<Simulator::CParticle<spDim>>& particles,
				std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents,
				std::vector<int>& particleCount,
				int index) {
				for (auto& particle : particles)
				{
					blaze::StaticVector<double, spDim> particleVector = blaze::abs(particle.Coords - CoordsAndExtents[index].first);
					bool checker = true;
					for (auto d : CoordsAndExtents[index].second)
					{
						checker = checker && (particleVector[0] <= d);
					}
					if (checker)
					{
						result[index] += particle.Velocity;
						particleCount[index] += 1;
					}
				}}, result, particles, CoordsAndExtents, particleCount, i));
		}
		std::for_each(workers.begin(), workers.end(), [](std::thread &t) { t.join(); });
	}

	template <size_t spDim>
	void GetDensityAveragedByExtents(
		const std::vector<Simulator::CParticle<spDim>>& particles,
		double square,
		std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents,
		std::vector<double>& result,
		std::vector<double>& squares)
	{
		result.resize(CoordsAndExtents.size());

		squares.resize(CoordsAndExtents.size());

		std::vector<std::thread> workers;
		for (int i = 0; i < CoordsAndExtents.size(); i++)
		{
			workers.push_back(std::thread(
				[](std::vector<double>& result,
				std::vector<Simulator::CParticle<spDim>>& particles,
				std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents,
				int index) {
				for (auto& particle : particles)
				{
					blaze::StaticVector<double, spDim> particleVector = blaze::abs(particle.Coords - CoordsAndExtents[index].first);
					bool checker = true;
					for (auto d : CoordsAndExtents[index].second)
					{
						checker = checker && (particleVector[0] <= d);
					}
					if (checker)
					{
						result[index] += 1;
					}
				}}, result, particles, CoordsAndExtents, i));

				squares[i] = CoordsAndExtents[i].second[0] * CoordsAndExtents[i].second[0];
		}

		std::for_each(workers.begin(), workers.end(), [](std::thread &t) { t.join(); });
	}

}
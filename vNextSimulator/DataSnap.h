#pragma once
#include "Simulator.h"
#include <thread>

template<size_t spDim>
class CDataSnap
{
	typedef std::tuple<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>, int> Coord_AreaExtent_Velocity_NumParticles;
	typedef std::tuple<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>, double, int> Coord_AreaExtent_Dencity_NumParticles;
public:
	template<size_t spDim>
	CDataSnap(Simulator::CSimulator<spDim>& s)
	{
		std::cout << "Creating data snap" << std::endl;
		SaveMiscInfo(s);
		SaveParticleData(s);
		SaveVelocityData(s);
		SaveDencityData(s);
	};
	~CDataSnap() {};

	int ParticleCount;
	double ParticleVelocity;
	int stepsFromStart;
	double Noise;
	blaze::StaticVector<double, spDim> Extents;
	std::string simGuid;
	std::string BorderConditions;
	std::string ParticleInterractions;
	std::string NoiseFunction;
	std::string StabilityChecker;

	//Previous noise, Steps with previous noises
	std::vector<std::pair<double, int>> PreviousSimulations;

	//Coordinate, Velocity
	std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>> ParticleData;
	
	//Coordinate, AreaExtents, Velocity, NumParticles
	std::vector<Coord_AreaExtent_Velocity_NumParticles> AverageVelocityData;

	//Coordinate, AreaExtents, Dencity, NumParticles
	std::vector<Coord_AreaExtent_Dencity_NumParticles> AverageDencityData;

	template<size_t spDim> void SaveMiscInfo(Simulator::CSimulator<spDim>& s)
	{
		simGuid = s.Guid;
		stepsFromStart = s.Steps;
		Noise = s.GetNoise();
		Extents = s.Extents;
		PreviousSimulations = std::vector<std::pair<double, int>>(s.PreviousNoises.begin(), s.PreviousNoises.end());
		ParticleCount = s.GetParticleCount();
		ParticleVelocity = s.GetParticleVelocity();
		ParticleInterractions = s.ParticleInterractions;
		NoiseFunction = s.NoiseFunction;
		BorderConditions = s.BorderConditions;
		StabilityChecker = s.StabilityChecker;
	};
	template<size_t spDim> void SaveParticleData(Simulator::CSimulator<spDim>& s)
	{
		auto particleDat = s.GetParticles();
		ParticleData.reserve(particleDat.size());
		for (auto& particle : particleDat)
		{
			ParticleData.push_back(std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>(
				blaze::StaticVector<double, spDim>(particle.Coords),
				blaze::StaticVector<double, spDim>(particle.Velocity)));
		}
	};
	template<size_t spDim> void SaveVelocityData(Simulator::CSimulator<spDim>& s)
	{
		SaveVelocityData(s, PrepareSquareExtents(s.Extents, Simulator::CSimulator<spDim>::NumOfMeaningfulSplits()));
	};
	template<size_t spDim> void SaveDencityData(Simulator::CSimulator<spDim>& s)
	{
		SaveDencityData(s, PrepareSquareExtents(s.Extents, Simulator::CSimulator<spDim>::NumOfMeaningfulSplits()));
	};

	template <size_t spDim>
	void GetParticleVelocityByExtents(Simulator::CSimulator<spDim>& s, std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents, std::vector<blaze::StaticVector<double, spDim>>& result, std::vector<int>& particleCount)
	{
		result.resize(CoordsAndExtents.size());

		particleCount.resize(CoordsAndExtents.size());

		auto particles = s.GetParticles();

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

						if (particleVector[0] <= CoordsAndExtents[index].second[0] &&
							particleVector[1] <= CoordsAndExtents[index].second[1])
						{
							result[index] += particle.Velocity;
							particleCount[index] += 1;
						}
					}}, std::ref(result), std::ref(particles), std::ref(CoordsAndExtents), std::ref(particleCount), i));
		}
		std::for_each(workers.begin(), workers.end(), [](std::thread &t) { t.join(); });
	}

	template<size_t spDim> void SaveVelocityData(Simulator::CSimulator<spDim>& s, std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents)
	{
		std::vector<blaze::StaticVector<double, spDim>> result;
		std::vector<int> particleCount;
		GetParticleVelocityByExtents<spDim>(s, CoordsAndExtents, result, particleCount);

		for (int i = 0; i < result.size(); i++)
		{
			AverageVelocityData.push_back(
				Coord_AreaExtent_Velocity_NumParticles(
				CoordsAndExtents[i].first, CoordsAndExtents[i].second, result[i] / particleCount[i], particleCount[i]));
		}
	};
	
	template<size_t spDim> void SaveDencityData(Simulator::CSimulator<spDim>& s, std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents)
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
				std::vector<Simulator::CParticle<spDim>>& particles,
				std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents,
				int index) {
				for (auto& particle : particles)
				{
					blaze::StaticVector<double, spDim> particleVector = blaze::abs(particle.Coords - CoordsAndExtents[index].first);
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
	};
	static std::vector<std::pair<blaze::Vec3d, blaze::Vec3d>> PrepareSquareExtents(blaze::Vec3d squareArea, int num)
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
	static std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> PrepareSquareExtents(blaze::Vec2d squareArea, int num)
	{
		std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> extents;

		double dX = squareArea[0] / (2.0 * num);
		double dY = squareArea[1] / (2.0 * num);

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
	static std::vector<std::pair<blaze::Vec3d, blaze::Vec3d>> PrepareExtents(blaze::Vec3d squareArea, blaze::Vec3d num)
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
	static std::vector<std::pair<blaze::Vec2d, blaze::Vec2d>> PrepareExtents(blaze::Vec2d squareArea, blaze::Vec2d num)
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
};
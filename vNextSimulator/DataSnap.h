#pragma once
#include "Simulator.h"
#include <thread>
#include "AveragingHelpers.h"

template<size_t spDim>
class CDataSnap
{
	typedef std::tuple<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>, int> Coord_AreaExtent_Velocity_NumParticles;
	typedef std::tuple<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>, double, int> Coord_AreaExtent_Dencity_NumParticles;
public:

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
	template<size_t spDim> 
	void SaveVelocityData(Simulator::CSimulator<spDim>& s)
	{
		auto tmp = AveragingHelpers::PrepareSquareExtents(s.Extents, AveragingHelpers::NumOfMeaningfulSplits(s.GetParticles().size()));
		SaveVelocityData(s, tmp);
	};

	template<size_t spDim> 
	void SaveDencityData(Simulator::CSimulator<spDim>& s)
	{
		auto tmp = AveragingHelpers::PrepareSquareExtents(s.Extents, AveragingHelpers::NumOfMeaningfulSplits(s.GetParticles().size()));
		SaveDencityData(s, tmp);
	};

	template<size_t spDim> 
	void SaveVelocityData(Simulator::CSimulator<spDim>& s,
		std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents)
	{
		std::vector<blaze::StaticVector<double, spDim>> result;
		std::vector<int> particleCount;
		AveragingHelpers::GetParticleVelocityAveragedByExtents(s.GetParticles(), CoordsAndExtents, result, particleCount);

		for (int i = 0; i < result.size(); i++)
		{
			AverageVelocityData.push_back(
				Coord_AreaExtent_Velocity_NumParticles(
				CoordsAndExtents[i].first, CoordsAndExtents[i].second, result[i] / particleCount[i], particleCount[i]));
		}
	};
	
	template<size_t spDim> void SaveDencityData(
		Simulator::CSimulator<spDim>& s,
		std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents)
	{
		std::vector<double> result;
		std::vector<double> squares;
		AveragingHelpers::GetDensityAveragedByExtents(
			s.GetParticles(),
			static_cast<double>(s.Extents[0] * s.Extents[1]),
			CoordsAndExtents, result, squares);

		for (int i = 0; i < CoordsAndExtents.size(); i++)
		{
			AverageDencityData.push_back(
				Coord_AreaExtent_Dencity_NumParticles(CoordsAndExtents[i].first, CoordsAndExtents[i].second, result[i] / squares[i], result[i]));
		}
	};
};
#pragma once
#include <thread>

#include <regex>
#include "boost\lexical_cast.hpp"

#include "DataSnap.h"
#include "JSONSaver.h"

#include "Simulator.h"

template<size_t spDim>
class CSimulationManager
{
	template<size_t spDim>
	class CSimulatorParams
	{
	public:
		double minNoise; double maxNoise; double noiseStep; int maxSteps;
		int particleCount; blaze::StaticVector<double, spDim> size; double particleVelocity;
		Simulator::EBorderConditions bCond;
		Simulator::EParticleInterractions ppInt;
		Simulator::EParticleNoise ptNoise;
		Simulator::EStabilityChecker stCheck;

		CSimulatorParams(double _minNoise, double _maxNoise, double _noiseStep,
			int _particleCount, blaze::StaticVector<double, spDim> _size, double _particleVelocity,
			Simulator::EBorderConditions _bCond,
			Simulator::EParticleInterractions _ppInt,
			Simulator::EParticleNoise _ptNoise,
			Simulator::EStabilityChecker _stCheck,
			int _maxSteps)
		{
			minNoise = _minNoise;
			maxNoise = _maxNoise;
			noiseStep = _noiseStep;
			particleCount = _particleCount;
			particleVelocity = _particleVelocity;
			size = _size;
			bCond = _bCond;
			ppInt = _ppInt;
			ptNoise = _ptNoise;
			stCheck = _stCheck;
			maxSteps = _maxSteps;
		};

		static CSimulatorParams GetDefault()
		{
			return CSimulatorParams(0, 360, 1, 1024, blaze::StaticVector<double, spDim>(32), 1,
						Simulator::EBorderConditions::RectangularTransitionalBC,
						Simulator::EParticleInterractions::VicsekInt,
						Simulator::EParticleNoise::UniformNoiseRotation,
						Simulator::EStabilityChecker::NumOfSteps, 5000);
		}
	};

public:
	std::string saveFileName = "AllDataFile.txt";

	CSimulationManager() {};
	CSimulationManager(int argc, const char **argv)
	{
		CreateParams(PrepareParams(argc, argv));
		Init();
	};

	void RunSimulations()
	{
		int notExit = 1;

		while (notExit)
		{
			notExit = 0;

			for (int i = 0; i < m_Simulators.size(); i++)
			{
				if (m_Simulators[i].GetNoise() >= m_SimulatorParams[i].minNoise &&
					m_Simulators[i].GetNoise() <= m_SimulatorParams[i].maxNoise)
				{
					notExit++;
					std::cout << " S." << i << " St." << m_Simulators[i].Steps <<
						" N." << m_SimulatorParams[i].minNoise << "<" << m_Simulators[i].GetNoise() << "<" << m_SimulatorParams[i].maxNoise <<
						" | ";
					if (m_Simulators[i].IsStable())
					{
						CJSONSaver::SaveAll(saveFileName, CDataSnap<spDim>(m_Simulators[i]));
						m_Simulators[i].ChangeNoise(m_SimulatorParams[i].noiseStep);
					}
					else
					{
						m_Simulators[i].Interract();
					}
				}
			}
			std::cout << std::endl << std::endl;
		}
	};

	~CSimulationManager() {};
private:
	int m_NumOfSimulators = 1;
	std::vector<CSimulatorParams<spDim>> m_SimulatorParams;
	std::vector<Simulator::CSimulator<spDim>> m_Simulators;

	std::vector<std::string> PrepareParams(int argc, const char **argv)
	{
		std::vector<std::string> ret;
		for (int i = 0; i < argc; i++)
		{
			std::string str(argv[i]);
			str += " ";
			if (str.find("--") != std::string::npos)
				ret.push_back(str);
			else
				ret[ret.size() - 1] += str;
		}
		for (auto& str : ret)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		}
		return ret;
	};

	void CreateParams(std::vector<std::string> argv)
	{
		std::cout << "Reading parameters" << std::endl;

		std::vector<double> minNoises;
		std::vector<double> maxNoises;
		std::vector<double> noiseSteps;
		std::vector<double> particleVelocities;
		std::vector<int> particleCounts;
		std::vector<double> sizesX;
		std::vector<double> sizesY;
		std::vector<double> sizesZ;
		std::vector<blaze::StaticVector<double, spDim>> sizes;
		std::vector<Simulator::EBorderConditions> bCond;
		std::vector<Simulator::EParticleInterractions> ppInt;
		std::vector<Simulator::EParticleNoise> ptNoise;
		std::vector<Simulator::EStabilityChecker> stCheck;
		std::vector<int> maxSteps;

		InterpreteConsoleParameters(argv,
			minNoises,
			maxNoises,
			noiseSteps,
			particleVelocities,
			particleCounts,
			sizesX,
			sizesY,
			sizesZ,
			sizes,
			bCond,
			ppInt,
			ptNoise,
			stCheck,
			maxSteps);

		FillWithDefaults(minNoises,
			maxNoises,
			noiseSteps,
			particleVelocities,
			particleCounts,
			sizesX,
			sizesY,
			sizesZ,
			sizes,
			bCond,
			ppInt,
			ptNoise,
			stCheck,
			maxSteps);
		
		for (int i = 0; i < m_NumOfSimulators; i++)
		{
			m_SimulatorParams.push_back(CSimulatorParams<spDim>(minNoises[i], maxNoises[i], noiseSteps[i],
				particleCounts[i], sizes[i], particleVelocities[i], bCond[i], ppInt[i], ptNoise[i], stCheck[i],
				maxSteps[i]));
		}
	};

	void InterpreteConsoleParameters(
		std::vector<std::string>& argv,
		std::vector<double>& minNoises,
		std::vector<double>& maxNoises,
		std::vector<double>& noiseSteps,
		std::vector<double>& particleVelocities,
		std::vector<int>& particleCounts,
		std::vector<double>& sizesX,
		std::vector<double>& sizesY,
		std::vector<double>& sizesZ,
		std::vector<blaze::StaticVector<double, spDim>>& sizes,
		std::vector<Simulator::EBorderConditions>& bCond,
		std::vector<Simulator::EParticleInterractions>& ppInt,
		std::vector<Simulator::EParticleNoise>& ptNoise,
		std::vector<Simulator::EStabilityChecker>& stCheck,
		std::vector<int>& maxSteps)
	{
		for (int i = 0; i < argv.size(); i++)
		{
			std::string str(argv[i]);
			std::smatch m;

			if (str.find("--numofsimulators") != std::string::npos)
			{
				std::regex_search(str, m, std::regex("--numofsimulators=(\\d{1,})"));
				m_NumOfSimulators = boost::lexical_cast<int>(m[1].str());
			}
			if (std::regex_search(str, m, std::regex("--numofparticles=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}) ")))
				{
					particleCounts.push_back(boost::lexical_cast<int>(subM[1].str()));
					group = subM.suffix().str();
				}
				continue;
			}
			if (std::regex_search(str, m, std::regex("--maxnoise=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[,.]?\\d{0,}) ")))
				{
					maxNoises.push_back(boost::lexical_cast<double>(subM[1].str()));
					group = subM.suffix().str();
				}
				continue;
			}
			if (std::regex_search(str, m, std::regex("--minnoise=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[,.]?\\d{0,}) ")))
				{
					minNoises.push_back(boost::lexical_cast<double>(subM[1].str()));
					group = subM.suffix().str();
				}
				continue;
			}
			if (std::regex_search(str, m, std::regex("--noisestep=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[,.]?\\d{0,}) ")))
				{
					noiseSteps.push_back(boost::lexical_cast<double>(subM[1].str()));
					group = subM.suffix().str();
				}
				continue;
			}
			if (std::regex_search(str, m, std::regex("--particlevelocity=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[,.]?\\d{0,}) ")))
				{
					particleVelocities.push_back(boost::lexical_cast<double>(subM[1].str()));
					group = subM.suffix().str();
				}
				continue;
			}
			if (std::regex_search(str, m, std::regex("--sizex=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[,.]?\\d{0,}) ")))
				{
					sizesX.push_back(boost::lexical_cast<double>(subM[1].str()));
					group = subM.suffix().str();
				}
				continue;
			}
			if (std::regex_search(str, m, std::regex("--sizey=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[,.]?\\d{0,}) ")))
				{
					sizesY.push_back(boost::lexical_cast<double>(subM[1].str()));
					group = subM.suffix().str();
				}
				continue;
			}
			if (std::regex_search(str, m, std::regex("--sizez=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[,.]?\\d{0,}) ")))
				{
					sizesZ.push_back(boost::lexical_cast<double>(subM[1].str()));
					group = subM.suffix().str();
				}
				continue;
			}
			if (std::regex_search(str, m, std::regex("--borderconditions=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(.*) ")))
				{
					if (subM[1] == "transitional")
						bCond.push_back(Simulator::EBorderConditions::RectangularTransitionalBC);
					if (subM[1] == "kuette")
						bCond.push_back(Simulator::EBorderConditions::RectangularKuetteBC);
					if (subM[1] == "kuetterough")
						bCond.push_back(Simulator::EBorderConditions::RectangularRoughKuetteBC);
					if (subM[1] == "kuettedouble")
						bCond.push_back(Simulator::EBorderConditions::RectangularTwoSidedKuetteBC);
					group = subM.suffix().str();
				}
				continue;
			}
			if (std::regex_search(str, m, std::regex("--stabilitycheck=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(.*) ")))
				{
					if (subM[1] == "maxsteps")
						stCheck.push_back(Simulator::EStabilityChecker::NumOfSteps);
					if (subM[1] == "averagevelocity")
						stCheck.push_back(Simulator::EStabilityChecker::AverageVelocity);
					if (subM[1] == "avveldispersionx")
						stCheck.push_back(Simulator::EStabilityChecker::AvVelDispersionX);
					if (subM[1] == "avveldispersiony")
						stCheck.push_back(Simulator::EStabilityChecker::AvVelDispersionY);
					if (subM[1] == "avveldispersionz")
						stCheck.push_back(Simulator::EStabilityChecker::AvVelDispersionZ);
					if (subM[1] == "avveldispersionxy")
						stCheck.push_back(Simulator::EStabilityChecker::AvVelDispersionXY);
					if (subM[1] == "avveldispersionyz")
						stCheck.push_back(Simulator::EStabilityChecker::AvVelDispresionYZ);
					if (subM[1] == "avveldispersionxyz")
						stCheck.push_back(Simulator::EStabilityChecker::AvVelDispersionXYZ);

					group = subM.suffix().str();
				}
				continue;
			}
		}
	};

	void FillWithDefaults(std::vector<double>& minNoises,
		std::vector<double>& maxNoises,
		std::vector<double>& noiseSteps,
		std::vector<double>& particleVelocities,
		std::vector<int>& particleCounts,
		std::vector<double>& sizesX,
		std::vector<double>& sizesY,
		std::vector<double>& sizesZ,
		std::vector<blaze::StaticVector<double, spDim>>& sizes,
		std::vector<Simulator::EBorderConditions>& bCond,
		std::vector<Simulator::EParticleInterractions>& ppInt,
		std::vector<Simulator::EParticleNoise>& ptNoise,
		std::vector<Simulator::EStabilityChecker>& stCheck,
		std::vector<int>& maxSteps)
	{
		CSimulatorParams<spDim> def = CSimulatorParams<spDim>::GetDefault();
		if (minNoises.size() == 0)
			minNoises.push_back(def.minNoise);
		for (int i = minNoises.size(); i < m_NumOfSimulators; i++)
			minNoises.push_back(minNoises[0]);

		if (maxNoises.size() == 0)
			maxNoises.push_back(def.maxNoise);
		for (int i = maxNoises.size(); i < m_NumOfSimulators; i++)
			maxNoises.push_back(maxNoises[0]);

		if (noiseSteps.size() == 0)
			noiseSteps.push_back(def.noiseStep);
		for (int i = noiseSteps.size(); i < m_NumOfSimulators; i++)
			noiseSteps.push_back(noiseSteps[0]);

		if (particleVelocities.size() == 0)
			particleVelocities.push_back(def.particleVelocity);
		for (int i = particleVelocities.size(); i < m_NumOfSimulators; i++)
			particleVelocities.push_back(particleVelocities[0]);

		if (particleCounts.size() == 0)
			particleCounts.push_back(def.particleCount);
		for (int i = particleCounts.size(); i < m_NumOfSimulators; i++)
			particleCounts.push_back(particleCounts[0]);

		CreateSizes(sizesX, sizesY, sizesZ, sizes, def);

		if (bCond.size() == 0)
			bCond.push_back(def.bCond);
		for (int i = bCond.size(); i < m_NumOfSimulators; i++)
			bCond.push_back(bCond[0]);

		if (ppInt.size() == 0)
			ppInt.push_back(def.ppInt);
		for (int i = ppInt.size(); i < m_NumOfSimulators; i++)
			ppInt.push_back(ppInt[0]);

		if (ptNoise.size() == 0)
			ptNoise.push_back(def.ptNoise);
		for (int i = ptNoise.size(); i < m_NumOfSimulators; i++)
			ptNoise.push_back(ptNoise[0]);

		if (stCheck.size() == 0)
			stCheck.push_back(def.stCheck);
		for (int i = stCheck.size(); i < m_NumOfSimulators; i++)
			stCheck.push_back(stCheck[0]);

		if (maxSteps.size() == 0)
			maxSteps.push_back(def.maxSteps);
		for (int i = maxSteps.size(); i < m_NumOfSimulators; i++)
			maxSteps.push_back(maxSteps[0]);
	}

	void CreateSizes(std::vector<double>& sX,
		std::vector<double>& sY,
		std::vector<double>& sZ,
		std::vector<blaze::StaticVector<double, 2UL>>& s,
		CSimulatorParams<2UL>& def)
	{
		if (sX.size() == 0)
			sX.push_back(def.size[0]);
		if (sY.size() == 0)
			sY.push_back(def.size[1]);

		for (int i = sX.size(); i < m_NumOfSimulators; i++)
			sX.push_back(sX[0]);
		for (int i = sY.size(); i < m_NumOfSimulators; i++)
			sY.push_back(sY[0]);

		for (int i = 0; i < m_NumOfSimulators; i++)
			s.push_back(blaze::StaticVector<double, 2UL>(sX[i], sY[i]));
	};

	void CreateSizes(std::vector<double>& sX,
		std::vector<double>& sY,
		std::vector<double>& sZ,
		std::vector<blaze::StaticVector<double, 3UL>>& s,
		CSimulatorParams<3UL>& def)
	{
		if (sX.size() == 0)
			sX.push_back(def.size[0]);
		if (sY.size() == 0)
			sY.push_back(def.size[1]);
		if (sZ.size() == 0)
			sZ.push_back(def.size[2]);

		for (int i = sX.size(); i < m_NumOfSimulators; i++)
			sX.push_back(sX[0]);
		for (int i = sY.size(); i < m_NumOfSimulators; i++)
			sY.push_back(sY[0]);
		for (int i = sZ.size(); i < m_NumOfSimulators; i++)
			sZ.push_back(sZ[0]);

		for (int i = 0; i < m_NumOfSimulators; i++)
			s.push_back(blaze::StaticVector<double, 3UL>(sX[i], sY[i], sZ[i]));
	};

	void Init()
	{
		std::cout << "Creating simulators" << std::endl;
		for (int i = 0; i < m_SimulatorParams.size(); i++)
		{
			m_Simulators.push_back(Simulator::CSimulator<spDim>(
				m_SimulatorParams[i].particleCount,
				m_SimulatorParams[i].size,
				m_SimulatorParams[i].particleVelocity,
				m_SimulatorParams[i].ppInt,
				m_SimulatorParams[i].bCond,
				m_SimulatorParams[i].ptNoise,
				m_SimulatorParams[i].stCheck,
				m_SimulatorParams[i].maxSteps,
				m_SimulatorParams[i].noiseStep > 0 ? m_SimulatorParams[i].minNoise : m_SimulatorParams[i].maxNoise
				));
		}
		if (m_Simulators.size() == 0)
		{
			std::cout << "Creating defaults" << std::endl;
		}
		std::cout << "Initialisation finished" << std::endl;
	};
};


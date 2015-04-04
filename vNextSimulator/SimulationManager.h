#pragma once
#include <thread>

#include <regex>
#include "boost\lexical_cast.hpp"
#include "SimulationManager.h"

#include "DataSnap.h"
#include "JSONSaver.h"

#include "Simulator.h"

#include "VicsecPPInterractor.h"
#include "UniformNoiseRotation.h"
#include "RectangularTransitionalBorders.h"
#include "RectangularRoughKuetteBorders.h"
#include "RectangularKuetteBorders.h"
#include "RectangularTwoSidedKuetteBorders.h"

template<size_t spDim>
class CSimulationManager
{
	enum EBorderConditions
	{
		RectangularTransitionalBC,
		RectangularKuetteBC,
		RectangularRoughKuetteBC,
		RectangularTwoSidedKuetteBC
	};

	enum EParticleInterractions
	{
		VicsekInt
	};

	enum EParticleNoise
	{
		UniformRandomRotation
	};

	template<size_t spDim>
	class CSimulatorParams
	{
	public:
		double minNoise; double maxNoise; double noiseStep;
		int particleCount; blaze::StaticVector<double, spDim> size; double particleVelocity;
		EBorderConditions bCond;
		std::function<void(Simulator::CParticle<spDim>&)> pbInterract;
		EParticleInterractions ppInt;
		std::function<void(Simulator::CParticle<spDim>&, Simulator::CParticle<spDim>&)> ppInterract;
		EParticleNoise ptNoise;
		std::function<void(Simulator::CParticle<spDim>&, double)> noiseFunc;

		CSimulatorParams(double _minNoise, double _maxNoise, double _noiseStep,
			int _particleCount, blaze::StaticVector<double, spDim> _size, double _particleVelocity,
			std::function<void(Simulator::CParticle<spDim>&, Simulator::CParticle<spDim>&)> _ppInterract,
			std::function<void(Simulator::CParticle<spDim>&)> _pbInterract,
			std::function<void(Simulator::CParticle<spDim>&, double)> _noiseFunc)
		{
			minNoise = _minNoise;
			maxNoise = _maxNoise;
			noiseStep = _noiseStep;
			particleCount = _particleCount;
			size = _size;
			ppInterract = _ppInterract;
			pbInterract = _pbInterract;
			noiseFunc = _noiseFunc;
		};

		static CSimulatorParams GetDefault()
		{
			auto tmp = CSimulatorParams(0, 360, 1, 1024, blaze::StaticVector<double, spDim>(32), 1,
				Simulator::CVicsecPPInterractor(),
				Simulator::CRectangularTransitionalBorders<spDim>(blaze::StaticVector<double, spDim>(32)),
				Simulator::CUniformNoiseRotation());

			tmp.bCond = RectangularTransitionalBC;
			tmp.ppInt = VicsekInt;
			tmp.ptNoise = UniformRandomRotation;
			return tmp;
		}
	};

public:
	std::string saveFileName = "AllDataFile.txt";

	CSimulationManager() {};
	CSimulationManager(int argc, const char **argv)
	{
		if (argc > 1)
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
						" N" << m_SimulatorParams[i].minNoise << "<" << m_Simulators[i].GetNoise() << "<" << m_SimulatorParams[i].maxNoise <<
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
		for (int i = 1; i < argc; i++)
		{
			std::string str(argv[i]);
			str += " ";
			if (str.find("--") != std::string::npos)
				ret.push_back(str);
			else
				ret[ret.size() - 1] += str;
		}
		return ret;
	};

	void CreateParams(std::vector<std::string> argv)
	{
		std::cout << "Reading parameters" << std::endl;

		std::string str(argv[0]);
		std::smatch m;
		if (str.find("--numOfSimulators") != std::string::npos)
		{
			std::regex_search(str, m, std::regex("--numOfSimulators=(\\d{1,})"));
			m_NumOfSimulators = boost::lexical_cast<int>(m[1].str());
		}

		std::vector<double> minNoises;
		std::vector<double> maxNoises;
		std::vector<double> noiseSteps;
		std::vector<double> particleVelocities;
		std::vector<int> particleCounts;
		std::vector<double> sizesX;
		std::vector<double> sizesY;
		std::vector<double> sizesZ;
		std::vector<blaze::StaticVector<double, spDim>> sizes;
		std::vector<EBorderConditions> bCond;
		std::vector<std::function<void(Simulator::CParticle<spDim>&)>> bcFunctions;
		std::vector<EParticleInterractions> ppInt;
		std::vector<std::function<void(Simulator::CParticle<spDim>&, Simulator::CParticle<spDim>&)>> ppIntFunctions;
		std::vector<EParticleNoise> ptNoise;
		std::vector<std::function<void(Simulator::CParticle<spDim>&, double)>> noiseFunctions;

		for (int i = 1; i < argv.size(); i++)
		{
			std::string str(argv[i]);
			std::smatch m;

			if (std::regex_search(str, m, std::regex("--numOfParticles=(.*)")))
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
			if (std::regex_search(str, m, std::regex("--maxNoise=(.*)")))
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
			if (std::regex_search(str, m, std::regex("--minNoise=(.*)")))
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
			if (std::regex_search(str, m, std::regex("--noiseStep=(.*)")))
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
			if (std::regex_search(str, m, std::regex("--particleVelocity=(.*)")))
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
			if (std::regex_search(str, m, std::regex("--sizeX=(.*)")))
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
			if (std::regex_search(str, m, std::regex("--sizeY=(.*)")))
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
			if (std::regex_search(str, m, std::regex("--sizeZ=(.*)")))
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
			if (std::regex_search(str, m, std::regex("--borderConditions=(.*)")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(.*) ")))
				{
					if (subM[1] == "transitional")
						bCond.push_back(RectangularTransitionalBC);
					if (subM[1] == "kuette")
						bCond.push_back(RectangularKuetteBC);
					if (subM[1] == "kuetteRough")
						bCond.push_back(RectangularRoughKuetteBC);
					if (subM[1] == "kuetteDouble")
						bCond.push_back(RectangularTwoSidedKuetteBC);
					group = subM.suffix().str();
				}
				continue;
			}
		}

		//!!!!!!!!!!!!!!!!!! end FOR
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
		CreateBorderConditionFunctions(bCond, bcFunctions, sizes, def);
		CreatePPIntFunctions(ppInt, ppIntFunctions, def);
		CreateNoiseFunctions(ptNoise, noiseFunctions, def);
	};

	void CreateBorderConditionFunctions(std::vector<EBorderConditions>& bCond,
		std::vector<std::function<void(Simulator::CParticle<spDim>&)>>& bcFunctions,
		std::vector<blaze::StaticVector<double, spDim>>& sizes,
		CSimulatorParams<spDim>& def)
	{
		if (bCond.size() == 0)
			bCond.push_back(def.bCond);
		for (int i = bCond.size(); i < m_NumOfSimulators; i++)
			bCond.push_back(bCond[0]);

		for (int i = 0; i < bCond.size(); i++)
		{
			switch (bCond[i])
			{
			case RectangularTransitionalBC:
				bcFunctions.push_back(Simulator::CRectangularTransitionalBorders<spDim>(sizes[i]));
				break;
			case RectangularKuetteBC:
				bcFunctions.push_back(Simulator::CRectangularKuetteBorders<spDim>(sizes[i]));
					break;
			case RectangularRoughKuetteBC:
				//bcFunctions.push_back(Simulator::CRectangularRoughKuetteBorders<spDim>(sizes[i]));
					break;
			case RectangularTwoSidedKuetteBC:
				bcFunctions.push_back(Simulator::CRectangularTwoSidedKuetteBorders<spDim>(sizes[i]));
				break;
			default:
				std::cout << "Unknown border condition parameter" << std::endl;
				break;
			}
		}
	}

	void CreatePPIntFunctions(std::vector<EParticleInterractions>& ppInt,
		std::vector<std::function<void(Simulator::CParticle<spDim>&, Simulator::CParticle<spDim>&)>> &ppIntFunctions,
		CSimulatorParams<spDim>& def)
	{
		if (ppInt.size() == 0)
			ppInt.push_back(def.ppInt);
		for (int i = ppInt.size(); i < m_NumOfSimulators; i++)
			ppInt.push_back(ppInt[0]);

		for (int i = 0; i < ppInt.size(); i++)
		{
			switch (ppInt[i])
			{
			case VicsekInt:
				ppIntFunctions.push_back(Simulator::CVicsecPPInterractor());
			default:
				std::cout << "Unknown particle-particle interraction parameter" << std::endl;
				break;
			}
		}
	}
	
	void CreateNoiseFunctions(std::vector<EParticleNoise>& ptNoise,
		std::vector<std::function<void(Simulator::CParticle<spDim>&, double)>> &noiseFunctions,
		CSimulatorParams<spDim>& def)
	{
		if (ptNoise.size() == 0)
			ptNoise.push_back(def.ptNoise);
		for (int i = ptNoise.size(); i < m_NumOfSimulators; i++)
			ptNoise.push_back(ptNoise[0]);

		for (int i = 0; i < ptNoise.size(); i++)
		{
			switch (ptNoise[i])
			{
			case UniformRandomRotation:
				noiseFunctions.push_back(Simulator::CUniformNoiseRotation());
			default:
				break;
			}
		}
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
				m_SimulatorParams[i].ppInterract,
				m_SimulatorParams[i].pbInterract,
				m_SimulatorParams[i].noiseFunc,
				m_SimulatorParams[i].noiseStep > 0 ? m_SimulatorParams[i].minNoise : m_SimulatorParams[i].maxNoise
				));
		}
	};
};


#include "stdafx.h"
#include <regex>
#include "boost\lexical_cast.hpp"
#include "SimulationManager.h"
#include "JSONSaver.h"

CSimulationManager::CSimulationManager(int argc, const char **argv)
{
	if (argc > 1)
		CreateParams(PrepareParams(argc, argv));
	Init();
}

std::vector<std::string> CSimulationManager::PrepareParams(int argc, const char **argv)
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
}

void CSimulationManager::CreateParams(std::vector<std::string> argv)
{
	std::cout << "Reading parameters" << std::endl;
	int numOfSimulators = 1;

	std::string str(argv[0]);
	std::smatch m;
	if (str.find("--numOfSimulators") != std::string::npos)
	{
		std::regex_search(str, m, std::regex("--numOfSimulators=(\\d{1,})"));
		numOfSimulators = boost::lexical_cast<int>(m[1].str());
	}

	for (int i = 0; i < numOfSimulators; i++)
	{
		m_SimulatorParams.push_back(SSimulatorParams());
	}

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
				m_SimulatorParams[i++].particleCount = boost::lexical_cast<int>(subM[1].str());
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
				m_SimulatorParams[i++].maxNoise = boost::lexical_cast<double>(subM[1].str());
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
				m_SimulatorParams[i++].minNoise = boost::lexical_cast<double>(subM[1].str());
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
				m_SimulatorParams[i++].noiseStep = boost::lexical_cast<double>(subM[1].str());
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
				m_SimulatorParams[i++].particleVelocity = boost::lexical_cast<double>(subM[1].str());
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
				m_SimulatorParams[i++].sizeX = boost::lexical_cast<double>(subM[1].str());
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
				m_SimulatorParams[i++].sizeY = boost::lexical_cast<double>(subM[1].str());
				group = subM.suffix().str();
			}
			continue;
		}
	}
}

void CSimulationManager::Init()
{
	std::cout << "Creating simulators" << std::endl;
	for (int i = 0; i < m_SimulatorParams.size(); i++)
	{
		m_Simulators.push_back(Simulator::CSimulator(
			m_SimulatorParams[i].particleCount,
			m_SimulatorParams[i].sizeX,
			m_SimulatorParams[i].sizeY,
			m_SimulatorParams[i].ppInterract,
			m_SimulatorParams[i].pbInterract,
			m_SimulatorParams[i].noiseFunc,
			m_SimulatorParams[i].noiseStep > 0 ? m_SimulatorParams[i].minNoise : m_SimulatorParams[i].maxNoise
			));
	}
}

void CSimulationManager::RunSimulations()
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
					CJSONSaver::SaveAll(saveFileName, CDataSnap(m_Simulators[i]));
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
}

CSimulationManager::~CSimulationManager()
{}

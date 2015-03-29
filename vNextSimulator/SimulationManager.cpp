#include "stdafx.h"
#include <regex>
#include "boost\lexical_cast.hpp"
#include "SimulationManager.h"

CSimulationManager::CSimulationManager(int argc, const char **argv)
{
	CreateParams(argc, argv);
	Init();
}

void CSimulationManager::CreateParams(int argc, const char **argv)
{
	int numOfSimulators = 1;

	std::string str(argv[1]);
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

	for (int i = 2; i < argc; i++)
	{
		std::string str(argv[i]);
		std::smatch m;
		if (str.find("--numOfParticles") != std::string::npos)
		{
			if(std::regex_search(str, m, std::regex("--numOfParticles=(.*?)--")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(.*?) ")))
				{
					m_SimulatorParams[i++].particleCount = boost::lexical_cast<int>(subM[1].str());
					group = subM.suffix().str();
				}
			}
		}
		else if (str.find("--maxNoise") != std::string::npos)
		{
			if (std::regex_search(str, m, std::regex("--maxNoise=(.*?)--")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[.,]?\\d{0,}) ")))
				{
					m_SimulatorParams[i++].maxNoise = boost::lexical_cast<double>(subM[1].str());
					group = subM.suffix().str();
				}
			}
		}
		else if (str.find("--minNoise") != std::string::npos)
		{
			if (std::regex_search(str, m, std::regex("--minNoise=(.*?)--")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[.,]?\\d{0,}) ")))
				{
					m_SimulatorParams[i++].minNoise = boost::lexical_cast<double>(subM[1].str());
					group = subM.suffix().str();
				}
			}
		}
		else if (str.find("--noiseStep") != std::string::npos)
		{
			if (std::regex_search(str, m, std::regex("--noiseStep=(.*?)--")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[.,]?\\d{0,}) ")))
				{
					m_SimulatorParams[i++].noiseStep = boost::lexical_cast<double>(subM[1].str());
					group = subM.suffix().str();
				}
			}
		}
		else if (str.find("--particleVelocity") != std::string::npos)
		{
			if (std::regex_search(str, m, std::regex("--particleVelocity=(.*?)--")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[.,]?\\d{0,}) ")))
				{
					m_SimulatorParams[i++].particleVelocity = boost::lexical_cast<double>(subM[1].str());
					group = subM.suffix().str();
				}
			}
		}
		else if (str.find("--sizeX") != std::string::npos)
		{
			if (std::regex_search(str, m, std::regex("--sizeX=(.*?)--")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[.,]?\\d{0,}) ")))
				{
					m_SimulatorParams[i++].sizeX = boost::lexical_cast<double>(subM[1].str());
					group = subM.suffix().str();
				}
			}
		}
		else if (str.find("--sizeY") != std::string::npos)
		{
			if (std::regex_search(str, m, std::regex("--sizeY=(.*?)--")))
			{
				auto group = m[1].str();
				std::smatch subM;
				int i = 0;
				while (std::regex_search(group, subM, std::regex("(\\d{1,}[.,]?\\d{0,}) ")))
				{
					m_SimulatorParams[i++].sizeY = boost::lexical_cast<double>(subM[1].str());
					group = subM.suffix().str();
				}
			}
		}
	}
}

void CSimulationManager::Init()
{

}

CSimulationManager::~CSimulationManager()
{}

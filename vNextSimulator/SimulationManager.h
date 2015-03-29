#pragma once
#include "Simulator.h"

#include "VicsecPPInterractor.h"
#include "UniformNoiseRotation.h"
#include "RectangularTransitionalBorders.h"

struct SSimulatorParams
{
	double minNoise; double maxNoise; double noiseStep;
	int particleCount; double sizeX; double sizeY; double particleVelocity;
	std::function<void(Simulator::CParticle&, Simulator::CParticle&)> ppInterract;
	std::function<void(Simulator::CParticle&)> pbInterract;
	std::function<void(Simulator::CParticle&, double)> noiseFunc;

	SSimulatorParams()
	{
		minNoise = 0; maxNoise = 360; noiseStep = 1;
		particleCount = 1024; sizeX = 32; sizeX = 32;
		ppInterract = Simulator::CVicsecPPInterractor();
		pbInterract = Simulator::CRectangularTransitionalBorders(sizeX, sizeX);
		noiseFunc = Simulator::CUniformNoiseRotation();
	}
};

class CSimulationManager
{
public:
	CSimulationManager(int argc, const char **argv);
	~CSimulationManager();

private:
	std::vector<SSimulatorParams> m_SimulatorParams;
	std::vector<Simulator::CSimulator> m_Simulators;
	void CreateParams(int argc, const char **argv);
	void Init();
};


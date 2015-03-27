#pragma once

#include <vector>
#include <functional>
#include <objbase.h>

#include "Particle.h"

namespace Simulator
{
	class CSimulator
	{
	public:
		int Steps;
		std::string Guid;
		blaze::Vec2d Extents;
		
		//Previous noise, Steps with previous noises
		std::vector<std::pair<double, int>> PreviousNoises;
		
		void Interract();
		void ChangeNoise(double degree);
		const std::vector<CParticle>& GetParticles();

		CSimulator(int particleCount, double maxX, double maxY,
			std::function<void(CParticle, CParticle)> ppInterract,
			std::function<void(CParticle)> pbInterract,
			std::function<void(CParticle, double)> noiseFunc);
		~CSimulator();
	private:
		int m_ParticleCount;
		double m_Noise;
		std::vector<CParticle> m_ParticlesOld;
		std::vector<CParticle> m_ParticlesNew;
		std::function<void(CParticle, CParticle)> m_ParticleParticleInterract;
		std::function<void(CParticle)> m_ParticleBorderInterract;
		std::function<void(CParticle, double)> m_NoiseFunction;

		void CreateGuid();
	};

}
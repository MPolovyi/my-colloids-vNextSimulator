#pragma once

#include <vector>
#include <functional>
#include <objbase.h>

#include "Particle.h"

namespace Simulator
{
	class CSimulator
	{
		friend class CAvVelStabilityCheck;
	public:
		int Steps;
		std::string Guid;
		blaze::Vec2d Extents;
		
		//Previous noise, Steps with previous noises
		std::vector<std::pair<double, int>> PreviousNoises;
		
		void Interract();
		double GetNoise();
		double GetParticleVelocity();
		double GetParticleCount();
		void ChangeNoise(double degree);
		const std::vector<CParticle>& GetParticles();

		CSimulator(int particleCount, double maxX, double maxY,
			std::function<void(CParticle&, CParticle&)> ppInterract,
			std::function<void(CParticle&)> pbInterract,
			std::function<void(CParticle&, double)> noiseFunc);
		~CSimulator();
	private:
		int m_ParticleCount;
		double m_Noise;
		double m_ParticleVelocity = 0.1;
		std::vector<CParticle> m_ParticlesOld;
		std::vector<CParticle> m_ParticlesNew;
		std::function<void(CParticle&, CParticle&)> m_ParticleParticleInterract;
		std::function<void(CParticle&)> m_ParticleBorderInterract;
		std::function<void(CParticle&, double)> m_NoiseFunction;

		void CreateGuid();
	};

}
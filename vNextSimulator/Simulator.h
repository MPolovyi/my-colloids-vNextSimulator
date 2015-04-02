#pragma once

#include <vector>
#include <functional>
#include <objbase.h>

#include "Particle.h"

namespace Simulator
{
	template<size_t spDim>
	class CSimulator
	{
	public:
		typedef CParticle<spDim> CParticle;
		int Steps;
		std::string Guid;
		blaze::StaticVector<double, spDim> Extents;
		
		//Previous noise, Steps with previous noises
		std::vector<std::pair<double, int>> PreviousNoises;
		
		void Interract()
		{
			concurrency::parallel_for(0, m_ParticleCount, [&](int i) {
				m_ParticlesNew[i].Coords = m_ParticlesOld[i].Coords;
				m_ParticlesNew[i].Velocity = m_ParticlesOld[i].Velocity;
				for (int j = 0; j < m_ParticleCount; j++)
				{
					if (m_ParticlesNew[i] != m_ParticlesOld[j])
						m_ParticleParticleInterract(m_ParticlesNew[i], m_ParticlesOld[j]);
				}
			});

			concurrency::parallel_for(0, m_ParticleCount, [&](int i) {
				m_ParticlesNew[i].Velocity = blaze::normalize(m_ParticlesNew[i].Velocity);
				m_ParticlesNew[i].Coords += m_ParticlesNew[i].Velocity * m_ParticleVelocity;
				m_ParticleBorderInterract(m_ParticlesNew[i]);
				m_NoiseFunction(m_ParticlesNew[i], m_Noise);
			});

			std::swap(m_ParticlesNew, m_ParticlesOld);
			Steps++;
		};
		bool IsStable()
		{
			if (Steps > 1000)
				return true;
			else
				return false;
		};
		double GetNoise() { return m_Noise; };
		double GetParticleVelocity() { return m_ParticleVelocity; };
		double GetParticleCount() { return m_ParticleCount; };
		void ChangeNoise(double degree)
		{
			PreviousNoises.push_back(std::pair<double, int>(m_Noise, Steps));
			m_Noise += degree;
			Steps = 0;
		};

		const std::vector<CParticle>& GetParticles()
		{
			return m_ParticlesOld;
		};

		template<size_t spDim>
		CSimulator(int particleCount, blaze::StaticVector<double, spDim> extents,
			std::function<void(CParticle&, CParticle&)> ppInterract,
			std::function<void(CParticle&)> pbInterract,
			std::function<void(CParticle&, double)> noiseFunc, double initNoise)
		{
			auto guid = CreateGuid();
			auto ptIndexShufle = guid.Data2 + guid.Data3 +
				guid.Data4[0] + guid.Data4[1] + guid.Data4[2];
			m_ParticleCount = particleCount;
			for (int i = 0; i < m_ParticleCount; i++)
			{
				m_ParticlesOld.push_back(CParticle(ptIndexShufle + i, extents));
				m_ParticlesNew.push_back(CParticle(ptIndexShufle + i, extents));
			}

			Extents = extents;

			m_ParticleParticleInterract = ppInterract;
			m_ParticleBorderInterract = pbInterract;
			m_NoiseFunction = noiseFunc;
			m_Noise = initNoise;
			Steps = 0;
		};

		~CSimulator() {};
	private:
		int m_ParticleCount;
		double m_Noise;
		double m_ParticleVelocity = 0.1;
		std::vector<CParticle> m_ParticlesOld;
		std::vector<CParticle> m_ParticlesNew;
		std::function<void(CParticle&, CParticle&)> m_ParticleParticleInterract;
		std::function<void(CParticle&)> m_ParticleBorderInterract;
		std::function<void(CParticle&, double)> m_NoiseFunction;

		GUID CreateGuid()
		{
			GUID guid;
			CoCreateGuid(&guid);

			char buff[100];
			sprintf(buff, "%s", "Hello");

			sprintf_s(buff, 100, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
				guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
			Guid = buff;

			return guid;
		};

	};

}
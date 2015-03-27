#include "stdafx.h"
#include "Simulator.h"

namespace Simulator
{
	CSimulator::CSimulator(int particleCount)
	{
		CoCreateGuid(pGuid);

		m_ParticleCount = particleCount;
		for (int i = 0; i < m_ParticleCount; i++)
		{
			m_ParticlesOld.push_back(CParticle(i));
		}
	}

	void CSimulator::Interract()
	{
		for (int i = 0; i < m_ParticleCount; i++)
		{
			for (int j = 0; j < m_ParticleCount; j++)
			{
				if (m_ParticlesNew[i] != m_ParticlesOld[j])
					m_ParticleParticleInterract(m_ParticlesNew[i], m_ParticlesOld[j]);
			}
		}

		for (int i = 0; i < m_ParticleCount; i++)
		{
			m_ParticlesNew[i].Velocity.scale(1.0f);
			m_ParticleBorderInterract(m_ParticlesNew[i]);
			m_NoiseFunction(m_ParticlesNew[i], m_Noise);
		}
		Steps++;
	}

	void CSimulator::ChangeNoise(double degree)
	{
		PreviousNoises.push_back(std::pair<double, int>(m_Noise, Steps));
		m_Noise = degree;
		Steps = 0;
	}

	const std::vector<CParticle>& CSimulator::GetParticles()
	{
		return m_ParticlesOld;
	}
	
	CSimulator::~CSimulator()
	{
		delete pGuid;
	}
}
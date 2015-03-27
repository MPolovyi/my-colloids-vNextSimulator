#include "stdafx.h"
#include "Simulator.h"

namespace Simulator
{
	CSimulator::CSimulator(int particleCount, double maxX, double maxY,
		std::function<void(CParticle, CParticle)> ppInterract,
		std::function<void(CParticle)> pbInterract,
		std::function<void(CParticle, double)> noiseFunc)
	{
		CreateGuid();

		m_ParticleCount = particleCount;
		for (int i = 0; i < m_ParticleCount; i++)
		{
			m_ParticlesOld.push_back(CParticle(i, maxX, maxY)); 
			m_ParticlesNew.push_back(CParticle(i, maxX, maxY));
		}

		Extents = blaze::Vec2d(maxX, maxY);

		m_ParticleParticleInterract = ppInterract;
		m_ParticleBorderInterract = pbInterract;
		m_NoiseFunction = noiseFunc;
	}

	void CSimulator::CreateGuid()
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
			m_ParticlesNew[i].Velocity = blaze::normalize(m_ParticlesNew[i].Velocity);
			m_ParticlesNew[i].Coords += m_ParticlesNew[i].Velocity * 0.1;
			m_ParticleBorderInterract(m_ParticlesNew[i]);
			m_NoiseFunction(m_ParticlesNew[i], m_Noise);
		}
		std::swap(m_ParticlesNew, m_ParticlesOld);
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
	}
}
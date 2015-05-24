#pragma once
#include "Particle.h"

namespace Simulator
{
	template<size_t spDim>
	class CMaxStepsStabilityChecker
	{
	public:
		CMaxStepsStabilityChecker(int maxSteps) { m_MaxSteps = maxSteps; };
		virtual bool operator()(std::vector<CParticle<spDim>>& particles)
		{
			if (++m_TotalSteps > m_MaxSteps)
			{
				m_TotalSteps = 0;
				return true;
			}
			else
				return false;
		}
		virtual ~CMaxStepsStabilityChecker() {};
	protected:
		int m_TotalSteps;
		int m_MaxSteps;
	};
}
#pragma once
#include "MaxStepsStabilityChecker.h"

namespace Simulator
{
	template<size_t spDim>
	class CAverageVelocityStabilityChecker :
		public CMaxStepsStabilityChecker<spDim>
	{
	public:
		virtual bool operator()(std::vector<CParticle<spDim>>& particles) override
		{
			//TODO: add reduction
			if (++m_CycleSteps > m_MaxSteps)
			{
				m_CycleSteps = 0;
				return true;
			}
			else
			{
				if (m_CycleSteps > m_MaxCycleSteps)
				{
					m_CycleSteps = 0;
					blaze::StaticVector<double, spDim> sumVelocity;
					for (auto& pt : particles)
					{
						sumVelocity += pt.Velocity;
					}
					double averVelocity = blaze::length(sumVelocity / particles.size());
					if ((averVelocity - m_PrevAverVelocity) * sqrt(particles.size()) < 1)
					{
						return true;
					}
				}
				return false;
			}
		}

		CAverageVelocityStabilityChecker(int maxSteps, int maxCycle) : CMaxStepsStabilityChecker(maxSteps)
		{
			m_MaxCycleSteps = maxCycle;
		};
		virtual ~CAverageVelocityStabilityChecker() {};
	
	protected:
		double m_PrevAverVelocity;
		int m_MaxCycleSteps;
	};

}
#pragma once
#include "MaxStepsStabilityChecker.h"

namespace Simulator
{
	template<size_t spDim>
	class CAverageVelocityStabilityChecker :
		public CMaxStepsStabilityChecker<spDim>
	{
	public:
		virtual bool operator()(CSimulator<spDim> sim) override
		{
			//TODO: add reduction

			if (++m_CycleSteps > m_MaxCycleSteps)
			{
				auto particles = sim.GetParticles();
				m_CycleSteps = 0;
				blaze::StaticVector<double, spDim> sumVelocity;
				for (auto& pt : particles)
				{
					sumVelocity += pt.Velocity;
				}
				double averVelocity = blaze::length(sumVelocity / particles.size());
				bool checker = (averVelocity - m_PrevAverVelocity) * sqrt(particles.size()) < 1;
				m_PrevAverVelocity = averVelocity;

				return checker || CMaxStepsStabilityChecker<spDim>::operator()(sim);
			}
			else
				return false;
		}

		CAverageVelocityStabilityChecker(int maxSteps, int maxCycle) : CMaxStepsStabilityChecker(maxSteps)
		{
			m_MaxCycleSteps = maxCycle;
		};
		virtual ~CAverageVelocityStabilityChecker() {};
	
	protected:
		double m_PrevAverVelocity;
		int m_CycleSteps;
		int m_MaxCycleSteps;
	};

}
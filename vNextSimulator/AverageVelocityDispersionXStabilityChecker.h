#pragma once
#include "AverageVelocityStabilityChecker.h"
#include "AveragingHelpers.h"

namespace Simulator
{
	template<size_t spDim>
	class CAverageVelocityDispersionXStabilityChecker :
		public CAverageVelocityStabilityChecker<spDim>
	{
	public:
		CAverageVelocityDispersionXStabilityChecker(int maxSteps, int maxCycle, blaze::StaticVector<double, spDim> extents) :
			CAverageVelocityStabilityChecker(maxSteps, maxCycle), m_Extents(extents)
		{
			
		};

		virtual bool operator()(std::vector<CParticle<spDim>>& particles) override
		{
			if (m_CycleSteps > m_MaxCycleSteps)
			{
				auto dispersion = GetDispersion(particles);
				m_CycleSteps=0;
				bool checker = true;
				for(int i = 0; i < dispersion.size(); i++)
				{
					checker = checker && (dispersion[i] - m_PrevDispersion[i]) * sqrt(particles.size()) < 1;
				}
				m_PrevDispersion = dispersion;
				return checker || CMaxStepsStabilityChecker<spDim>::operator()(particles);
			}
			else
				return false;
		}


		blaze::StaticVector<double, spDim> GetDispersion(std::vector<CParticle<spDim>>& particles)
		{
			InitNumOfSplits(particles.size());
			auto CoordsAndExtents = AveragingHelpers::PrepareExtents(m_Extents, m_NumOfSplits);
			
			std::vector<blaze::StaticVector<double, spDim>> velocities;
			
			std::vector<int> particleCount;
			
			AveragingHelpers::GetParticleVelocityAveragedByExtents(
				particles, CoordsAndExtents, velocities, particleCount);

			concurrency::parallel_for(size_t(0), velocities.size(), [&](int i) {
				velocities[i] /= particleCount[i] != 0 ? particleCount[i] : 1;
			});

			blaze::StaticVector<double, spDim> disper;
			blaze::StaticVector<double, spDim> summ;
			
			for (size_t i = 0; i < velocities.size(); i++)
			{
				disper += velocities[i] * velocities[i];
				summ += velocities[i];
			}
			disper = (disper + (summ * summ) / velocities.size()) / velocities.size();
			
			return disper;
		}

		virtual ~CAverageVelocityDispersionXStabilityChecker(){};
	protected:

		virtual void InitNumOfSplits(int numOfParticles)
		{
			double* a = new double[spDim];

			a[0] = AveragingHelpers::NumOfMeaningfulSplits(numOfParticles);
			for (int i = 1; i < spDim; i++)
			{
				a[i] = 0;
			}

			m_NumOfSplits = blaze::StaticVector<double, spDim>(spDim, a);
		}

		blaze::StaticVector<double, spDim> m_PrevDispersion;
		blaze::StaticVector<double, spDim> m_Extents;
		blaze::StaticVector<double, spDim> m_NumOfSplits;
	};

}
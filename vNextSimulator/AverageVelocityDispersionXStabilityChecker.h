#pragma once
#include "AverageVelocityStabilityChecker.h"
#include "DataSnap.h"

namespace Simulator
{
	template<size_t spDim>
	class CAverageVelocityDispersionXStabilityChecker :
		public CAverageVelocityStabilityChecker<spDim>
	{
	public:
		CAverageVelocityDispersionXStabilityChecker(int maxSteps, int maxCycle) :
			CAverageVelocityStabilityChecker(maxSteps, maxCycle) {};

		virtual bool operator()(CSimulator<spDim>& sim) override
		{
			if (m_CycleSteps > m_MaxCycleSteps)
			{
				auto dispersion = GetDispersion(sim);
				m_CycleSteps=0;
				bool checker = true;
				for(int i = 0; i < dispersion.size(); i++)
				{
					checker = checker && (dispersion[i] - m_PrevDispersion[i]) * sqrt(particles.size()) < 1
				}
				m_PrevDispersion = dispersion;
				return checker || CMaxStepsStabilityChecker<spDim>::operator()(sim);
			}
			else
				return false;
		}

		virtual blaze::StaticVector<double, spDim> GetDispersion(CSimulator<spDim>& sim)
		{
			auto particles = sim.GetParticles();
			auto CoordsAndExtents = CDataSnap<spDim>::PrepareExtents(sim.Extents, CSimulator<spDim>::NumOfMeaningfulSplits(), 0);
			
			std::vector<blaze::StaticVector<double, spDim>> velocities;
			velocities.resize(CoordsAndExtents.size());

			std::vector<int> particleCount;
			particleCount.resize(CoordsAndExtents.size());
			
			std::vector<std::thread> workers;
			for (int i = 0; i < CoordsAndExtents.size(); i++)
			{
				workers.push_back(std::thread(
					[](std::vector<blaze::StaticVector<double, spDim>>& velocities,
					std::vector<CParticle<spDim>>& particles,
					std::vector<std::pair<blaze::StaticVector<double, spDim>, blaze::StaticVector<double, spDim>>>& CoordsAndExtents,
					std::vector<int>& particleCount,
					int index) {
					for (auto& particle : particles)
					{
						blaze::StaticVector<double, spDim> particleVector = blaze::abs(particle.Coords - CoordsAndExtents[index].first);

						if (particleVector[0] <= CoordsAndExtents[index].second[0] &&
							particleVector[1] <= CoordsAndExtents[index].second[1])
						{
							velocities[index] += particle.Velocity;
							particleCount[index] += 1;
						}
					}}, std::ref(velocities), std::ref(particles), std::ref(CoordsAndExtents), std::ref(particleCount), i));
			}
			std::for_each(workers.begin(), workers.end(), [](std::thread &t) { t.join(); });

			concurrency::parallel_for(0, velocities.size(), [&](int i){
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
	private:
		blaze::StaticVector<double, spDim> m_PrevDispersion;
	};

}
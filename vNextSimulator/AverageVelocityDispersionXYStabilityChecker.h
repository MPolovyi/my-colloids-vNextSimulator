#pragma once
#include "AverageVelocityDispersionXStabilityChecker.h"

namespace Simulator
{
	template<size_t spDim>
	class CAverageVelocityDispersionXYStabilityChecker :
		public CAverageVelocityDispersionXStabilityChecker<spDim>
	{
	public:
		CAverageVelocityDispersionXYStabilityChecker(int maxSteps, int maxCycle, blaze::StaticVector<double, spDim> extents) :
			CAverageVelocityDispersionXStabilityChecker(maxSteps, maxCycle, extents)
		{};
		virtual ~CAverageVelocityDispersionXYStabilityChecker(){};
	protected:
		virtual void InitNumOfSplits(int numOfParticles) override
		{
				double* a = new double[spDim];

				for (int i = 1; i < spDim; i++)
				{
					a[i] = AveragingHelpers::NumOfMeaningfulSplits(numOfParticles);
				}

				m_NumOfSplits = blaze::StaticVector<double, spDim>(spDim, a);
			}
	};
	
	template<size_t spDim>
	using CAverageVelocityDispersionXYZStabilityChecker = CAverageVelocityDispersionXYStabilityChecker<spDim>;
}
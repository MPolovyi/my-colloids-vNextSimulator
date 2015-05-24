#pragma once
#include "AverageVelocityDispersionXStabilityChecker.h"

namespace Simulator
{
	template<size_t spDim>
	class CAverageVelocityDispersionYStabilityChecker :
		public CAverageVelocityDispersionXStabilityChecker<spDim>
	{
	public:
		CAverageVelocityDispersionYStabilityChecker(int maxSteps, int maxCycle, blaze::StaticVector<double, spDim> extents) :
			CAverageVelocityDispersionXStabilityChecker(maxSteps, maxCycle, extents)
		{};
		virtual void InitNumOfSplits(int numOfParticles)
		{
			double* a = new double[spDim];

			for (int i = 0; i < spDim; i++)
			{
				a[i] = 0;
			}
			a[1] = AveragingHelpers::NumOfMeaningfulSplits(numOfParticles);
			m_NumOfSplits = blaze::StaticVector<double, spDim>(spDim, a);
		}
		virtual ~CAverageVelocityDispersionYStabilityChecker(){};
	};
}
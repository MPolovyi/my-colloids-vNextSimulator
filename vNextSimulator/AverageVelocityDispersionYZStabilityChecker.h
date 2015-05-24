#pragma once
#include "AverageVelocityDispersionXStabilityChecker.h"

namespace Simulator
{
	class CAverageVelocityDispersionYZStabilityChecker :
		public CAverageVelocityDispersionXStabilityChecker<3UL>
	{
	public:
		CAverageVelocityDispersionYZStabilityChecker(int maxSteps, int maxCycle, blaze::StaticVector<double, 3UL> extents) :
			CAverageVelocityDispersionXStabilityChecker(maxSteps, maxCycle, extents)
		{};
		virtual ~CAverageVelocityDispersionYZStabilityChecker() {};
	protected:
		virtual void InitNumOfSplits(int numOfParticles) override
		{
			m_NumOfSplits = blaze::StaticVector<double, 3UL>(0, AveragingHelpers::NumOfMeaningfulSplits(numOfParticles), AveragingHelpers::NumOfMeaningfulSplits(numOfParticles));
		}
	};

}
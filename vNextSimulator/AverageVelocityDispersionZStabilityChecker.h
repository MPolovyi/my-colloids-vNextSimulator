#pragma once
#include "AverageVelocityDispersionXStabilityChecker.h"

namespace Simulator
{
	class CAverageVelocityDispersionZStabilityChecker :
		public CAverageVelocityDispersionXStabilityChecker<3UL>
	{
	public:
		CAverageVelocityDispersionZStabilityChecker(int maxSteps, int maxCycle, blaze::StaticVector<double, 3UL> extents) :
			CAverageVelocityDispersionXStabilityChecker(maxSteps, maxCycle, extents)
		{};
		virtual ~CAverageVelocityDispersionZStabilityChecker() {};
	protected:
		virtual void InitNumOfSplits(int numOfParticles) override
		{
			m_NumOfSplits = blaze::StaticVector<double, 3UL>(0, 0, AveragingHelpers::NumOfMeaningfulSplits(numOfParticles));
		}
	};
}
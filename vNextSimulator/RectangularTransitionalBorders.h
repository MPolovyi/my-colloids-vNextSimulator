#pragma once
#include "Particle.h"

namespace Simulator
{
	class CRectangularTransitionalBorders
	{
	public:
		CRectangularTransitionalBorders(double x, double y, double z = 0);
		void operator()(CParticle<2UL>& particle);
		void operator()(CParticle<3UL>& particle);
		~CRectangularTransitionalBorders();
	private:
		double m_X;
		double m_Y;
		double m_Z;
	};
}
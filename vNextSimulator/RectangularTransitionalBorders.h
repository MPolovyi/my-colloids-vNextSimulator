#pragma once
#include "Particle.h"

namespace Simulator
{
	class CRectangularTransitionalBorders
	{
	public:
		CRectangularTransitionalBorders(double x, double y);
		void operator()(CParticle& particle);
		~CRectangularTransitionalBorders();
	private:
		double m_X;
		double m_Y;
	};
}
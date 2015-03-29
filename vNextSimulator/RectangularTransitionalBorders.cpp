#include "stdafx.h"
#include "RectangularTransitionalBorders.h"

namespace Simulator
{
	CRectangularTransitionalBorders::CRectangularTransitionalBorders(double x, double y)
	{
		m_X = x; m_Y = y;
	}

	void CRectangularTransitionalBorders::operator()(CParticle& particle)
	{
		auto xCoord = particle.Coords[0];
		auto yCoord = particle.Coords[1];
		if (xCoord < 0)
			particle.Coords[0] += m_X;
		if (xCoord > m_X)
			particle.Coords[0] -= m_X;
		if (yCoord < 0)
			particle.Coords[1] += m_Y;
		if (xCoord > m_Y)
			particle.Coords[1] -= m_Y;
	}

	CRectangularTransitionalBorders::~CRectangularTransitionalBorders()
	{}
}
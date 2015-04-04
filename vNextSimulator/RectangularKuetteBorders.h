#pragma once
#include "RectangularTransitionalBorders.h"

namespace Simulator
{
	template<size_t spDim>
	class CRectangularKuetteBorders :
		public CRectangularTransitionalBorders<spDim>
	{
	public:
		double Velocity;
		CRectangularKuetteBorders(blaze::StaticVector<double, spDim> size, double velocity = 1.0)
			: CRectangularTransitionalBorders(size)
		{
			Velocity = velocity;
		};
		void operator()(CParticle<2UL>& particle)
		{
			auto xCoord = particle.Coords[0];
			auto yCoord = particle.Coords[1];
			if (xCoord < 0)
				particle.Coords[0] += m_size[0];
			if (xCoord > m_size[0])
				particle.Coords[0] -= m_size[0];
			if (yCoord < 1)
			{
				double dist = particle.Coords[1] + particle.Velocity[1];
				if (dist > 0)
				{
					particle.Coords[1] -= 2 * dist;
					particle.Velocity[1] = -particle.Velocity[1];
				}
			}
			if (yCoord > m_size[1] - 1)
			{
				particle.Velocity[1] += Velocity;
				particle.Velocity = blaze::normalize(particle.Velocity);
				double dist = particle.Coords[1] + particle.Velocity[1] - m_size[1];
				if (dist > 0)
				{
					particle.Coords[1] -= 2 * dist;
					particle.Velocity[1] = -particle.Velocity[1];
				}
			}
		};
		void operator()(CParticle<3UL>& particle)
		{
			auto xCoord = particle.Coords[0];
			auto yCoord = particle.Coords[1];
			auto zCoord = particle.Coords[2];
			if (xCoord < 0)
				particle.Coords[0] += m_size[0];
			if (xCoord > m_size[0])
				particle.Coords[0] -= m_size[0];
			if (yCoord < 1)
			{
				double dist = particle.Coords[1] + particle.Velocity[1];
				if (dist > 0)
				{
					particle.Coords[1] -= 2 * dist;
					particle.Velocity[1] = -particle.Velocity[1];
				}
			}
			if (yCoord > m_size[1] - 1)
			{
				particle.Velocity[1] += Velocity;
				particle.Velocity = blaze::normalize(particle.Velocity);
				double dist = particle.Coords[1] + particle.Velocity[1] - m_size[1];
				if (dist > 0)
				{
					particle.Coords[1] -= 2 * dist;
					particle.Velocity[1] = -particle.Velocity[1];
				}
			}
			if (zCoord < 0)
				particle.Coords[2] += m_size[2];
			if (zCoord > m_size[2])
				particle.Coords[2] -= m_size[2];
		};
		~CRectangularKuetteBorders() {};
	};

}
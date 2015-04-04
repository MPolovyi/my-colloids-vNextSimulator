#pragma once
#include "RectangularKuetteBorders.h"

namespace Simulator
{
	template<size_t spDim>
	class CRectangularRoughKuetteBorders :
		public CRectangularKuetteBorders<spDim>
	{
	public:
		CRectangularRoughKuetteBorders(blaze::StaticVector<double, spDim> size, double velocity = 1.0)
			: CRectangularKuetteBorders(size, velocity)
		{
			CRectangularRoughKuetteBorders();
		};

		CRectangularRoughKuetteBorders(const CRectangularRoughKuetteBorders& rhs)
		{
			rndGen.seed(rhs.m_rndSeed, 8);
			Velocity = rhs.Velocity;
			m_size = rhs.m_size;
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
				particle.Velocity[1] *= rndGen();
				particle.Velocity = blaze::normalize(particle.Velocity);
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
				particle.Velocity[1] *= rndGen();
				particle.Velocity = blaze::normalize(particle.Velocity);
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
		~CRectangularRoughKuetteBorders() {};
	private:
		CRectangularRoughKuetteBorders()
		{
			GUID guid;
			CoCreateGuid(&guid);
			for (int i = 0; i < 8; i++)
			{
				m_rndSeed[i] = guid.Data4[i];
			}
			rndGen.seed(m_rndSeed, 8);
		}
		unsigned long m_rndSeed[8];
		MTRand rndGen;
	};
}
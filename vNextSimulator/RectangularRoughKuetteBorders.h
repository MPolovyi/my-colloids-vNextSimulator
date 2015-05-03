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
			GUID guid;
			CoCreateGuid(&guid);
			for (int i = 0; i < 8; i++)
			{
				m_rndSeed[i] = guid.Data4[i];
			}
			rndGen.seed(m_rndSeed, 8);
		};

		CRectangularRoughKuetteBorders(const CRectangularRoughKuetteBorders& rhs) : CRectangularKuetteBorders(rhs)
		{
			rndGen.seed(rhs.m_rndSeed, 8);
			Velocity = rhs.Velocity;
			m_size = rhs.m_size;
		};

		virtual void operator()(CParticle<2UL>& particle) override
		{
			auto yCoord = particle.Coords[1];

			if (yCoord > m_size[1] - 1)
			{
				particle.Velocity[1] *= rndGen();
				particle.Velocity = blaze::normalize(particle.Velocity);
			}
			CRectangularKuetteBorders<spDim>::operator()(particle);
		};

		virtual void operator()(CParticle<3UL>& particle) override
		{
			auto yCoord = particle.Coords[1];

			if (yCoord > m_size[1] - 1)
			{
				particle.Velocity[1] *= rndGen();
				particle.Velocity = blaze::normalize(particle.Velocity);
			}
			CRectangularKuetteBorders<spDim>::operator()(particle);
		};
		virtual ~CRectangularRoughKuetteBorders() {};
	private:
		unsigned long m_rndSeed[8];
		MTRand rndGen;
	};
}
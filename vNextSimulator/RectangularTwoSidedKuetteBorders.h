#pragma once
#include "RectangularKuetteBorders.h"

namespace Simulator
{
	template<size_t spDim>
	class CRectangularTwoSidedKuetteBorders :
		public CRectangularKuetteBorders<spDim>
	{
	public:
		double VelocitySecond;
		CRectangularTwoSidedKuetteBorders(blaze::StaticVector<double, spDim> size, double velocity = 1.0, double velocitySecond = -1.0)
			: CRectangularKuetteBorders(size)
		{
			Velocity = velocity;
			VelocitySecond = velocitySecond;
		};
		virtual void operator()(CParticle<2UL>& particle) override
		{
			auto yCoord = particle.Coords[1];
			
			if (yCoord > m_size[1] - 1)
			{
				particle.Velocity[1] += VelocitySecond;
				particle.Velocity = blaze::normalize(particle.Velocity);
			}

			CRectangularKuetteBorders<spDim>::operator()(particle);
		};

		virtual void operator()(CParticle<3UL>& particle) override
		{
			auto yCoord = particle.Coords[1];

			if (yCoord > m_size[1] - 1)
			{
				particle.Velocity[1] += VelocitySecond;
				particle.Velocity = blaze::normalize(particle.Velocity);
			}

			CRectangularKuetteBorders<spDim>::operator()(particle);
		};
		virtual ~CRectangularTwoSidedKuetteBorders() {};
	};
}
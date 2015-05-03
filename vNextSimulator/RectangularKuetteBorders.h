#pragma once
#include "RectangularTransitionalBorders.h"
#include "RectangularHorisontalTransitionBorders.h"

namespace Simulator
{
	template<size_t spDim>
	class CRectangularKuetteBorders :
		public CRectangularHorisontalTransitionBorders<spDim>
	{
	public:
		double Velocity;
		CRectangularKuetteBorders(blaze::StaticVector<double, spDim> size, double velocity = 1.0)
			: CRectangularHorisontalTransitionBorders(size)
		{
			Velocity = velocity;
		};
		virtual void operator()(CParticle<2UL>& particle) override
		{
			auto yCoord = particle.Coords[1];

			if (yCoord < 1)
			{
				particle.Velocity[1] += Velocity;
				particle.Velocity = blaze::normalize(particle.Velocity);
			}
			CRectangularHorisontalTransitionBorders<spDim>::operator()(particle);
		};
		virtual void operator()(CParticle<3UL>& particle) override
		{
			auto yCoord = particle.Coords[1];

			if (yCoord < 1)
			{
				particle.Velocity[1] += Velocity;
				particle.Velocity = blaze::normalize(particle.Velocity);
			}

			CRectangularHorisontalTransitionBorders<spDim>::operator()(particle);
		};
		virtual ~CRectangularKuetteBorders() {};
	};

}
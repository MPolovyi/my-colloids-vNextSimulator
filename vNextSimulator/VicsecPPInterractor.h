#pragma once
#include "Particle.h"
namespace Simulator
{
	class CVicsecPPInterractor
	{
	public:
		double rInt = 1;
		template<size_t spDim>
		void operator()(CParticle<spDim>& a, CParticle<spDim>& b)
		{
			if (blaze::length(a.Coords - b.Coords) <= rInt)
			{
				a.Velocity += b.Velocity;
			}
		};
		CVicsecPPInterractor() {};
		~CVicsecPPInterractor() {};
	};

}
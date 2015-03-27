#pragma once
#include "Particle.h"
namespace Simulator
{
	class CVicsecPPInterractor
	{
	public:
		double rInt = 1;
		void operator()(CParticle, CParticle);
		CVicsecPPInterractor();
		~CVicsecPPInterractor();
	};

}
#pragma once
#include "Particle.h"
namespace Simulator
{
	class CUniformNoiseRotation
	{
		friend class CParticle;
	public:
		CUniformNoiseRotation();
		void operator()(CParticle a, double noise);
		~CUniformNoiseRotation();
	};
}
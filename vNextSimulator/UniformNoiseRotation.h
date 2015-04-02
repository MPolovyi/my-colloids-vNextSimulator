#pragma once
#include "Particle.h"
namespace Simulator
{
	class CUniformNoiseRotation
	{
	public:
		CUniformNoiseRotation();
		void operator()(CParticle<2UL>& a, double noise);
		void operator()(CParticle<3UL>& a, double noise);
		~CUniformNoiseRotation();
	};
}
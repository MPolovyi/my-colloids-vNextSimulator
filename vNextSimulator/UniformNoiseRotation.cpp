#include "stdafx.h"
#include "UniformNoiseRotation.h"

namespace Simulator
{
	CUniformNoiseRotation::CUniformNoiseRotation()
	{}

	void CUniformNoiseRotation::operator()(CParticle& particle, double noise)
	{
		auto radian = (particle.rndGen() - 0.5)*noise*3.1415926 / 180.0;

		blaze::Mat2x2d rotationMatrix = blaze::Mat2x2d(cos(radian), -sin(radian), sin(radian), cos(radian));

		particle.Velocity = rotationMatrix * particle.Velocity;
	}

	CUniformNoiseRotation::~CUniformNoiseRotation()
	{}
}
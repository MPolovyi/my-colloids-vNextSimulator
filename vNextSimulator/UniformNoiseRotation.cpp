#include "stdafx.h"
#include "UniformNoiseRotation.h"

namespace Simulator
{
	CUniformNoiseRotation::CUniformNoiseRotation()
	{}

	void CUniformNoiseRotation::operator()(CParticle<2UL>& particle, double noise)
	{
		auto radian = (particle.rndGen() - 0.5)*noise*3.1415926 / 180.0;

		blaze::Mat2x2d rotationMatrix = blaze::Mat2x2d(cos(radian), -sin(radian), sin(radian), cos(radian));

		particle.Velocity = rotationMatrix * particle.Velocity;
	}

	void CUniformNoiseRotation::operator()(CParticle<3UL>& particle, double noise)
	{
		auto radian = (particle.rndGen() - 0.5)*noise*3.1415926 / 180.0;
		double aX = particle.Velocity[0];
		double aY = particle.Velocity[1];
		double aZ = particle.Velocity[2];
		double del = aX == 0 ? aY : aX;
		double bX = -(aY + aZ) / del;
		double bY = 1 / bX;
		double bZ = 1 / bX;
		blaze::Mat3x3d rotationMatrix = blaze::Mat3x3d(
			cos(radian) + (1 - cos(radian))*bX*bX, (1 - cos(radian))*bX*bY - sin(radian)*bZ, (1 - cos(radian))*bX*bZ + sin(radian)*bY,
			(1 - cos(radian))*bY*bX + sin(radian)*bZ, cos(radian) + (1 - cos(radian))*bY*bY, (1 - cos(radian))*bY*bZ - sin(radian)*bX,
			(1 - cos(radian))*bZ*bX - sin(radian)*bY, (1 - cos(radian))*bZ*bY + sin(radian)*bY, cos(radian) + (1 - cos(radian))*bZ*bZ);

		particle.Velocity = rotationMatrix * particle.Velocity;
	}

	CUniformNoiseRotation::~CUniformNoiseRotation()
	{}
}
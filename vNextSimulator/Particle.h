#pragma once
#include "stdafx.h"
#include "mtrand.h"

namespace Simulator
{
	class CParticle
	{
	public:
		MTRand rndGen;
		blaze::Vec2d Coords;
		blaze::Vec2d Velocity;

		CParticle();
		CParticle(int Id);
		CParticle(int Id, double maxX, double maxY);

		void Reset(double maxX, double maxY);

		bool operator==(const CParticle& rhs);
		bool operator!=(const CParticle& rhs);
		CParticle(const CParticle& rhs);
		~CParticle();
	private:
		int Id;
	};
}
#include "stdafx.h"
#include <random>
#include "Particle.h"

namespace Simulator
{
	CParticle::CParticle() : CParticle(rand()) {}

	CParticle::CParticle(int id) : CParticle(id, 1.0, 1.0) {}

	CParticle::CParticle(int id, double maxX, double maxY)
	{
		rndGen.seed(id);
		Id = id;

		Reset(maxX, maxY);
	}

	CParticle::CParticle(const CParticle& rhs)
	{
		Coords = rhs.Coords;
		Velocity = rhs.Velocity;
		Id = rhs.Id;
		rndGen.seed(rhs.Id);
	}

	void CParticle::Reset(double maxX, double maxY)
	{
		Coords[0] = rndGen()*maxX;
		Coords[1] = rndGen()*maxY;

		Velocity[0] = rndGen();
		Velocity[1] = rndGen();
	}

	bool CParticle::operator==(const CParticle& rhs)
	{
		return Id == rhs.Id;
	}

	bool CParticle::operator!=(const CParticle& rhs)
	{
		return Id != rhs.Id;
	}

	CParticle::~CParticle()
	{}
}
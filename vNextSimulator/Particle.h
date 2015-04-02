#pragma once
#include "stdafx.h"
#include "mtrand.h"
#include <random>

namespace Simulator
{
	template<size_t spDim>
	class CParticle
	{
		typedef  blaze::StaticVector<double, spDim> Vector;
	public:
		MTRand rndGen;
		Vector Coords;
		Vector Velocity;

		CParticle() : CParticle(rand()) {};
		CParticle(int id) : CParticle(id, blaze::StaticVector<double, spDim>(1.0)) {};
		CParticle(int id, blaze::StaticVector<double, spDim> extents)
		{
			rndGen.seed(id);
			Id = id;
			Reset(extents);
		};

		CParticle(const CParticle& rhs)
		{
			Coords = rhs.Coords;
			Velocity = rhs.Velocity;
			Id = rhs.Id;
			rndGen.seed(rhs.Id);
		};

		void Reset(blaze::StaticVector<double, 2UL> extents)
		{
			Coords[0] = rndGen()*extents[0];
			Coords[1] = rndGen()*extents[1];

			Velocity[0] = 0.5 - rndGen();
			Velocity[1] = 0.5 - rndGen();
			Velocity = blaze::normalize(Velocity);
		};

		void Reset(blaze::StaticVector<double, 3UL> extents)
		{
			Coords[0] = rndGen()*extents[0];
			Coords[1] = rndGen()*extents[1];
			Coords[2] = rndGen()*extents[2];

			Velocity[0] = 0.5 - rndGen();
			Velocity[1] = 0.5 - rndGen();
			Velocity[2] = 0.5 - rndGen();
			Velocity = blaze::normalize(Velocity);
		};

		bool operator==(const CParticle& rhs) { return Id == rhs.Id; };
		bool operator!=(const CParticle& rhs) { return Id != rhs.Id; };
		~CParticle() {};
	private:
		int Id;
	};
}
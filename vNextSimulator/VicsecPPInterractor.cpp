#pragma once
#include "stdafx.h"
#include "VicsecPPInterractor.h"

namespace Simulator
{
	void CVicsecPPInterractor::operator()(CParticle a, CParticle b)
	{
		if (blaze::length(a.Coords - b.Coords) <= rInt)
		{
			a.Velocity += b.Velocity;
		}
	}

	CVicsecPPInterractor::CVicsecPPInterractor()
	{}


	CVicsecPPInterractor::~CVicsecPPInterractor()
	{}
}
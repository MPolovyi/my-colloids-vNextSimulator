#pragma once
#include "Particle.h"

namespace Simulator
{
	template<size_t spDim>
	class CRectangularTransitionalBorders
	{
	public:
		CRectangularTransitionalBorders(blaze::StaticVector<double, spDim> size) : m_size(size) {};

		void ChangeSize(int index, double size)
		{
			m_size[index] = size;
		};

		void operator()(CParticle<spDim>& particle)
		{
			for (int i = 0; i < particle.Coords.size(); i++)
			{
				if (particle.Coords[i] < 0)
					particle.Coords[i] += m_size[i];

				if (particle.Coords[i] > m_size[i])
					particle.Coords[i] -= m_size[i];
			}
		};
		~CRectangularTransitionalBorders() {};
	protected:
		blaze::StaticVector<double, spDim> m_size;
	};
}
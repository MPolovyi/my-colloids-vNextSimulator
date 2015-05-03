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

		virtual void operator()(CParticle<2UL>& particle)
		{
			ParticleInt(particle);
		};
		virtual void operator()(CParticle<3UL>& particle)
		{
			ParticleInt(particle);
		};
		virtual ~CRectangularTransitionalBorders() {};
	protected:
		blaze::StaticVector<double, spDim> m_size;

	private:
		template<size_t ptDim>
		void ParticleInt(CParticle<ptDim>& particle)
		{
			for (int i = 0; i < particle.Coords.size(); i++)
			{
				if (particle.Coords[i] < 0)
					particle.Coords[i] += m_size[i];

				if (particle.Coords[i] > m_size[i])
					particle.Coords[i] -= m_size[i];
			}
		}
	};
}
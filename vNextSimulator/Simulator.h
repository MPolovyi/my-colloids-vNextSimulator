#pragma once

#include <vector>
#include <functional>
#include <objbase.h>

#include "Particle.h"

#include "VicsecPPInterractor.h"

#include "UniformNoiseRotation.h"

#include "BorderConditions.h"
#include "StabilityCheckers.h"


namespace Simulator
{
	enum EBorderConditions
	{
		RectangularTransitionalBC,
		RectangularKuetteBC,
		RectangularRoughKuetteBC,
		RectangularTwoSidedKuetteBC
	};

	enum EParticleInterractions
	{
		VicsekInt
	};

	enum EParticleNoise
	{
		UniformNoiseRotation
	};

	enum EStabilityChecker
	{
		NumOfSteps,
		AverageVelocity,
		AvVelDispersionX,
		AvVelDispersionY,
		AvVelDispersionZ,
		AvVelDispersionXY,
		AvVelDispresionYZ,
		AvVelDispersionXYZ
	};

	template<size_t spDim>
	class CSimulator
	{
	public:
		int Steps;
		std::string Guid;

		std::string BorderConditions;
		std::string ParticleInterractions;
		std::string NoiseFunction;
		std::string StabilityChecker;
		blaze::StaticVector<double, spDim> Extents;

		//Previous noise, Steps with previous noises
		std::vector<std::pair<double, int>> PreviousNoises;

		CSimulator(int particleCount, blaze::StaticVector<double, spDim> extents, double ptVelocity,
			EParticleInterractions ppInterract,
			EBorderConditions pbInterract,
			EParticleNoise noiseFunc,
			EStabilityChecker stabilityCheck, int maxSteps,
			double initNoise)
		{
			auto guid = CreateGuid();
			auto ptIndexShufle = guid.Data2 + guid.Data3 +
				guid.Data4[0] + guid.Data4[1] + guid.Data4[2];
			m_ParticleCount = particleCount;
			for (int i = 0; i < m_ParticleCount; i++)
			{
				m_ParticlesOld.push_back(CParticle<spDim>(ptIndexShufle + i, extents));
				m_ParticlesNew.push_back(CParticle<spDim>(ptIndexShufle + i, extents));
			}

			m_ParticleVelocity = ptVelocity;
			Extents = extents;

			m_ParticleParticleInterract = CreatePPIntFunction(ppInterract);
			m_ParticleBorderInterract = CreateBorderConditionFunction(pbInterract);
			m_NoiseFunction = CreateNoiseFunction(noiseFunc);
			m_StabilityChecker = CreateStabilityChecker(stabilityCheck, maxSteps);
			m_Noise = initNoise;
			Steps = 0;
		};

		double GetNoise() { return m_Noise; };
		double GetParticleVelocity() { return m_ParticleVelocity; };
		double GetParticleCount() { return m_ParticleCount; };
		void ChangeNoise(double degree)
		{
			PreviousNoises.push_back(std::pair<double, int>(m_Noise, Steps));
			m_Noise += degree;
			Steps = 0;
		};

		void Interract()
		{
			concurrency::parallel_for(0, m_ParticleCount, [&](int i) {
				m_ParticlesNew[i].Coords = m_ParticlesOld[i].Coords;
				m_ParticlesNew[i].Velocity = m_ParticlesOld[i].Velocity;
				for (int j = 0; j < m_ParticleCount; j++)
				{
					if (m_ParticlesNew[i] != m_ParticlesOld[j])
						m_ParticleParticleInterract(m_ParticlesNew[i], m_ParticlesOld[j]);
				}
			});


			concurrency::parallel_for(0, m_ParticleCount, [&](int i) {
				m_ParticlesNew[i].Velocity = blaze::normalize(m_ParticlesNew[i].Velocity);
				m_ParticlesNew[i].Coords += m_ParticlesNew[i].Velocity * m_ParticleVelocity;

				m_ParticleBorderInterract(m_ParticlesNew[i]);
				m_NoiseFunction(m_ParticlesNew[i], m_Noise);
			});

			std::swap(m_ParticlesNew, m_ParticlesOld);
			Steps++;
		};

		bool IsStable()
		{
			return m_StabilityChecker(m_ParticlesNew);
		};

		const std::vector<CParticle<spDim>>& GetParticles()
		{
			return m_ParticlesOld;
		};

		~CSimulator() {};
	private:
		int m_ParticleCount;
		double m_Noise;
		double m_ParticleVelocity = 0.1;
		std::vector<CParticle<spDim>> m_ParticlesOld;
		std::vector<CParticle<spDim>> m_ParticlesNew;
		std::function<void(CParticle<spDim>&, CParticle<spDim>&)> m_ParticleParticleInterract;
		std::function<void(CParticle<spDim>&)> m_ParticleBorderInterract;
		std::function<void(CParticle<spDim>&, double)> m_NoiseFunction;
		std::function<bool(std::vector<CParticle<spDim>>&)> m_StabilityChecker;

		GUID CreateGuid()
		{
			GUID guid;
			CoCreateGuid(&guid);

			char buff[100];
			sprintf(buff, "%s", "Hello");

			sprintf_s(buff, 100, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
				guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
			Guid = buff;

			return guid;
		};

		std::function<void(Simulator::CParticle<spDim>&)>
			CreateBorderConditionFunction(EBorderConditions bCond)
		{
				switch (bCond)
				{
				case RectangularTransitionalBC:
					BorderConditions = "RectangularTransitionalBC";
					return Simulator::CRectangularTransitionalBorders<spDim>(Extents);
					break;
				case RectangularKuetteBC:
					BorderConditions = "RectangularKuetteBC";
					return Simulator::CRectangularKuetteBorders<spDim>(Extents);
					break;
				case RectangularRoughKuetteBC:
					BorderConditions = "RectangularRoughKuetteBC";
					return Simulator::CRectangularRoughKuetteBorders<spDim>(Extents);
					break;
				case RectangularTwoSidedKuetteBC:
					BorderConditions = "RectangularTwoSidedKuetteBC";
					return Simulator::CRectangularTwoSidedKuetteBorders<spDim>(Extents);
					break;
				default:
					BorderConditions = "RectangularTransitionalBC";
					std::cout << "Unknown border condition parameter" << std::endl;
					return Simulator::CRectangularTransitionalBorders<spDim>(Extents);
					break;
				}
			};

		std::function<void(CParticle<spDim>&, CParticle<spDim>&)>
			CreatePPIntFunction(EParticleInterractions ppInt)
		{
				switch (ppInt)
				{
				case VicsekInt:
					ParticleInterractions = "VicsekInt";
					return Simulator::CVicsecPPInterractor();
					break;
				default:
					ParticleInterractions = "VicsekInt";
					return Simulator::CVicsecPPInterractor();
					break;
				}
			};

		std::function<void(CParticle<spDim>&, double)>
			CreateNoiseFunction(EParticleNoise ptNoise)
		{
				switch (ptNoise)
				{
				case UniformNoiseRotation:
					NoiseFunction = "UniformRandomRotation";
					return Simulator::CUniformNoiseRotation();
				default:
					NoiseFunction = "UniformRandomRotation";
					return Simulator::CUniformNoiseRotation();
					break;
				}
			};

		std::function<bool(std::vector<CParticle<spDim>>&)>
			CreateStabilityChecker(EStabilityChecker stCheck, int maxSteps)
		{
			switch (stCheck)
			{
			case Simulator::NumOfSteps:
				StabilityChecker = "NumOfSteps";
				return CMaxStepsStabilityChecker<spDim>(maxSteps);
				break;
			case Simulator::AverageVelocity:
				StabilityChecker = "AverageVelocity";
				break;
			case Simulator::AvVelDispersionX:
				StabilityChecker = "AvVelDispersionX";
				break;
			case Simulator::AvVelDispersionY:
				StabilityChecker = "AvVelDispersionY";
				break;
			case Simulator::AvVelDispersionZ:
				StabilityChecker = "AvVelDispersionZ";
				break;
			case Simulator::AvVelDispersionXY:
				StabilityChecker = "AvVelDispersionXY";
				break;
			case Simulator::AvVelDispresionYZ:
				StabilityChecker = "AvVelDispresionYZ";
				break;
			case Simulator::AvVelDispersionXYZ:
				StabilityChecker = "AvVelDispersionXYZ";
				break;
			default:
				StabilityChecker = "NumOfSteps";
				return CMaxStepsStabilityChecker<spDim>(maxSteps);
				break;
			}
		}
	};
}
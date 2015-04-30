using System;
using System.Collections.Generic;
using MongoDB.Bson;
using Newtonsoft.Json.Linq;

namespace SimulatorGUI
{
    class SimulationData
    {
        public ObjectId Id;
        public Guid Guid;
        public int ParticleCount;
        public int SpaceDimension;
        public double[] SimulatorDimensions;
        public double ParticleVelocity;
        public double Noise;
        public int SimulationSteps;
        public EBorderConditions BorderConditions;
        public EParticleInterractions ParticleInterractions;
        public EParticleNoise NoiseFunction;
        public EStabilityChecker StabilityChecker;
        public List<PrevSim> PreviousSimulations;
        public CData Data;
        public SimulationData(JObject jObj)
        {
            var misc = jObj["Miscelaneous"];
            Guid = new Guid(misc["GUID"].ToString());
            ParticleCount = misc["ParticleCount"].ToObject<int>();
            SimulatorDimensions = misc["SimSize"].ToObject<double[]>();
            SpaceDimension = SimulatorDimensions.Length;
            ParticleVelocity = misc["ParticleVelocity"].ToObject<double>();
            Noise = misc["Noise"].ToObject<double>();
            SimulationSteps = misc["SimulationSteps"].ToObject<int>();
            BorderConditions = ParseEnum<EBorderConditions>(misc["BorderConditions"].ToString());
            ParticleInterractions = ParseEnum<EParticleInterractions>(misc["ParticleInterractions"].ToString());
            NoiseFunction = ParseEnum<EParticleNoise>(misc["NoiseFunction"].ToString());
            StabilityChecker = ParseEnum<EStabilityChecker>(misc["SystemStabilityCheck"].ToString());
            PreviousSimulations = misc["PreviousSimulations"].ToObject<List<PrevSim>>();

            Data = jObj["Data"].ToObject<CData>();
        }

        private static T ParseEnum<T>(string value)
        {
            return (T)Enum.Parse(typeof(T), value, true);
        }

        public enum EBorderConditions
        {
            RectangularTransitionalBC,
            RectangularKuetteBC,
            RectangularRoughKuetteBC,
            RectangularTwoSidedKuetteBC
        };
        public enum EParticleInterractions
        {
            VicsekInt
        };
        public enum EParticleNoise
        {
            UniformRandomRotation
        };
        public enum EStabilityChecker
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

        public class PrevSim
        {
            public double Noise { get; set; }
            public int Steps { get; set; }
        }

        public class ParticleData
        {
            public List<double> Coordinate { get; set; }
            public List<double> Velocity { get; set; }
        }

        public class AverageVelocityData
        {
            public List<double> Coordinate { get; set; }
            public List<double> Extents { get; set; }
            public List<double> Velocity { get; set; }
            public int LocalParticleCount { get; set; }
        }

        public class AverageDencityData
        {
            public List<double> Coordinate { get; set; }
            public List<double> Extents { get; set; }
            public double Dencity { get; set; }
            public int LocalParticleCount { get; set; }
        }

        public class CData
        {
            public List<ParticleData> ParticleData { get; set; }
            public List<AverageVelocityData> AverageVelocityData { get; set; }
            public List<AverageDencityData> AverageDencityData { get; set; }
        }
    }
}

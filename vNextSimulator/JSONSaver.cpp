#include "stdafx.h"
#include "JSONSaver.h"

void CJSONSaver::SaveAll(std::string fName, CDataSnap data)
{
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> wr(s);

	std::cout << std::endl << "Saving simultion " << data.simGuid << " data" << std::endl;
	wr.StartObject();
	{	
		std::cout << "Saving info" << std::endl;
		WriteMiscelaneousInfo(wr, data);
		std::cout << "Saving data" << std::endl;
		WriteData(wr, data);
	}
	wr.EndObject();
	std::cout << "Writing file" << std::endl;
	SaveInFile(fName, s);
	std::cout << "Finished saving" << std::endl;
}

void CJSONSaver::WriteMiscelaneousInfo(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap data)
{
	wr.String("Miscelaneous");
	wr.StartObject();
	{
		wr.String("GUID");
		wr.String(data.simGuid.c_str());
		wr.String("ParticleCount");
		wr.Int(data.ParticleCount);
		wr.String("ParticleVelocity");
		wr.Double(data.ParticleVelocity);
		wr.String("Noise");
		wr.Double(data.Noise);
		wr.String("SimulationSteps");
		wr.Int(data.stepsFromStart);

		wr.String("PreviousSimulations");
		wr.StartArray();
		{
			for (auto prevSim : data.PreviousSimulations)
			{
				wr.StartObject();
				wr.String("Noise");
				wr.Double(prevSim.first);
				wr.String("SimulationSteps");
				wr.Int(prevSim.second);
				wr.EndObject();
			}
		}
		wr.EndArray();
	}
	wr.EndObject();
}

void CJSONSaver::WriteData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap data)
{
	wr.String("Data");
	wr.StartObject();
	{
		WriteParticleData(wr, data);
		WriteVelocityData(wr, data);
		WriteDencityData(wr, data);
	}
	wr.EndObject();
}

void CJSONSaver::WriteParticleData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap data) 
{
	wr.String("ParticleData");
	wr.StartArray();
	{
		for (auto particle : data.ParticleData)
		{
			wr.StartObject();
			wr.String("Coordinate");
			wr.StartArray();
			for (auto d : particle.first)
				wr.Double(d);
			wr.EndArray();
			wr.String("Velocity");
			wr.StartArray();
			for (auto d : particle.second)
				wr.Double(d);
			wr.EndArray();
			wr.EndObject();
		}
	}
	wr.EndArray();
}

void CJSONSaver::WriteVelocityData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap data)
{
	wr.String("AverageVelocityData");
	wr.StartArray();
	{
		for (auto vData : data.AverageVelocityData)
		{
			wr.StartObject();
				wr.String("Coordinate");
				wr.StartArray();
				for (auto d : std::get<0>(vData))
					wr.Double(d);
				wr.EndArray();

				wr.String("Extents");
				wr.StartArray();
				for (auto d : std::get<1>(vData))
					wr.Double(d);
				wr.EndArray();

				wr.String("Velocity");
				wr.StartArray();
				for (auto d : std::get<2>(vData))
					wr.Double(d);
				wr.EndArray();

				wr.String("LocalParticleCount");
				wr.StartArray();
				wr.Double(std::get<3>(vData));
				wr.EndArray();
			wr.EndObject();
		}
	}
	wr.EndArray();
}

void CJSONSaver::WriteDencityData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap data)
{
	wr.String("AverageDencityData");
	wr.StartArray();
	{
		for (auto vData : data.AverageDencityData)
		{
			wr.StartObject();
			wr.String("Coordinate");
			wr.StartArray();
			for (auto d : std::get<0>(vData))
				wr.Double(d);
			wr.EndArray();

			wr.String("Extents");
			wr.StartArray();
			for (auto d : std::get<1>(vData))
				wr.Double(d);
			wr.EndArray();

			wr.String("Dencity");
			wr.StartArray();
			wr.Double(std::get<2>(vData));
			wr.EndArray();

			wr.String("LocalParticleCount");
			wr.StartArray();
			wr.Double(std::get<3>(vData));
			wr.EndArray();
			wr.EndObject();
		}
	}
	wr.EndArray();
}

void CJSONSaver::SaveInFile(std::string fileName, rapidjson::StringBuffer& buffer)
{
	std::fstream file;
	file.open(fileName, std::ios::app);
	file << buffer.GetString() << ",";
	file.close();
}

CJSONSaver::CJSONSaver()
{}


CJSONSaver::~CJSONSaver()
{}

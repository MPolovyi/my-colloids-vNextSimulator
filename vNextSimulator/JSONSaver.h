#pragma once
#include "rapidjson\prettywriter.h"
#include "DataSnap.h"
class CJSONSaver
{
public:
	static void SaveAll(std::string fileName, CDataSnap& data);
private:
	~CJSONSaver();
	CJSONSaver();
	static void WriteMiscelaneousInfo(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap& data);
	static void WriteData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap& data);
	static void WriteParticleData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap& data);
	static void WriteVelocityData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap& data);
	static void WriteDencityData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap& data);
	static void SaveInFile(std::string fileName, rapidjson::StringBuffer& buffer);
};


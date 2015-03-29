#pragma once
#include "rapidjson\prettywriter.h"
#include "DataSnap.h"
class CJSONSaver
{
public:
	CJSONSaver();
	void SaveAll(std::string fileName, CDataSnap data);
	~CJSONSaver();
private:
	void WriteMiscelaneousInfo(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap data);
	void WriteData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap data);
	void WriteParticleData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap data);
	void WriteVelocityData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap data);
	void WriteDencityData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& wr, CDataSnap data);
	void SaveInFile(std::string fileName, rapidjson::StringBuffer& buffer);
};


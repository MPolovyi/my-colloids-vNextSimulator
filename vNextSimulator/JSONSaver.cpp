#include "stdafx.h"
#include "JSONSaver.h"

void CJSONSaver::SaveInFile(std::string fileName, rapidjson::StringBuffer& buffer)
{
	std::fstream file;
	file.open(fileName, std::ios::app);
	file << buffer.GetString() << ",";
	file.close();
}

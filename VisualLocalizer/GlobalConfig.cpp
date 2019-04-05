#include "GlobalConfig.h"



GlobalConfig::GlobalConfig(std::string ymlPath) 
{
	fs.open(ymlPath, cv::FileStorage::READ);
	valid = true;
	if (!readConfig())
	{
		valid = false;
	}
}

bool GlobalConfig::readConfig()
{
	if (!fs.isOpened())
	{
		return false;
	}
	fs["PathRec"] >> pathRec;
	fs["PathTest"] >> pathTest;
	fs["Sequence"] >> sequenceNum;
	fs["Topology"] >> backendType;
	std::string disType;
	fs["DistanceType"] >> disType;
	if (disType=="cos")
	{
		distanceType = 1;
	}
	else
	{
		distanceType = 0;
	}
	fs["CostHidden"] >> costHidden;
	return true;
}

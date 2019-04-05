#pragma once
#include <vector>
#include "PicGnssFile.h"
#include "GlobalConfig.h"

class Descriptorbase
{
public:	
	Descriptorbase(GlobalConfig& config);
	virtual ~Descriptorbase() {};

	PicGnssFile picsRec;
	cv::Mat xCNNRec;

private:
	void loadFeatureFromFile(const std::string &filename, std::vector<unsigned char>& dim);

};


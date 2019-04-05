#pragma once
#include <vector>
#include "PicGnssFile.h"
#include "GlobalConfig.h"

class DescriptorQuery
{
public:
	DescriptorQuery(GlobalConfig& config);
	virtual ~DescriptorQuery() ;

	std::vector<std::string> pathsTest;
	cv::Mat getCNNQuery()const { return xCNNQuery; };
	
	PicGnssFile* picsTest;

private:
	/*CNN*/
	cv::Mat xCNNQuery;

	void loadFeatureFromFile(const std::string &filename, std::vector<unsigned char>& dim);
};
#pragma once
#include "picGnssfile.h"
#include "GlobalConfig.h"
#include "database.h"
#include "DescriptorQuery.h"
#include "graph\networkflow.h"

enum BackendType{NetworkFlowTopology, BruteForce, Ordinary};

class VisualLocalization
{
public:
	VisualLocalization(GlobalConfig& config);
	virtual ~VisualLocalization();	
	/// 针对于读入图像描述子（一般是ConvNet-based Descriptor的情况），进行视觉定位
	bool featureLocalize(const bool & outPR = true);
	
private:
	int backendType;
	int distanceType;
	double costHidden;

	// 训练集数据(保存记录的路径)
	Descriptorbase* descriptorbase;

	// 测试集数据
	DescriptorQuery* descriptorquery;
	std::vector<bool> keyGT, keyPredict, keyGPS;
	bool withGPS;
	std::string descriptor;

	// get a distance matrix, which is as follows
	//   ----> database
	//  |
	//  |
	//  V
	//  query images
	NetworkFlow* netFlow;

};

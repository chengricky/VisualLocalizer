#pragma once

#include <vector>
#include <string>
#include "Header.h"

class FileInterface
{
public:
	FileInterface() {}
	~FileInterface() {}
	virtual void init(std::vector<std::string> folderPath, int mode, bool ifLabeledCW) = 0;

	virtual bool doMain() = 0;
	virtual cv::Size getImgSize() = 0;
	virtual bool loadNextVideo() { return true; }

	virtual bool readVideo() = 0;

	cv::Mat colorImg;
	cv::Mat depthImg;
	cv::Mat IRImg;

	std::vector<cv::Point2d> vertexes;
	std::vector<int> frames;
};
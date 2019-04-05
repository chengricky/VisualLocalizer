#include "PicGnssFile.h"
#include <io.h>

using namespace std;

PicGnssFile::PicGnssFile(std::vector<std::string> filepaths, int mode, bool ifLabeled, int sequenceNum, std::string fileKeyWord)
{
	init(filepaths, mode, ifLabeled, sequenceNum, fileKeyWord);
}

void PicGnssFile::init(std::vector<std::string> filepaths, int mode, bool ifLabeled, std::string fileKeyWord)
{
	init(filepaths, mode, ifLabeled, 1, fileKeyWord);
}

void PicGnssFile::init(std::vector<std::string> filepaths, int mode, bool ifLabeled, int sequenceNum, std::string fileKeyWord)
{
	this->sequenceNum = sequenceNum;
	this->mode = mode;
	this->ifLabelCW = ifLabeled;
	vector<string> suffix;
	suffix.push_back("png");
	suffix.push_back("jpg");
	//文件句柄
	intptr_t hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	for (size_t j = 0; j < filepaths.size(); j++)
	{
		string filepath = filepaths[j];
		for (size_t i = 0; i < suffix.size(); i++)
		{			
			string searchPath = filepath + "\\" + fileKeyWord + "." + suffix[i];
			if ((hFile = _findfirst(searchPath.c_str(), &fileinfo)) != -1)
			{
				do
				{
					string filename(fileinfo.name);
					size_t pos = filename.find_first_of('c');
					string filePrefix = filename.substr(0U, pos);
					//检查除彩图之外其他图像或文件是否存在
					bool depth, ground;
					bool label = ifLabeled;
					if (mode == RGB)
					{
						depth = false;
						ground = false;
						findFilesfromColor(filepath, filePrefix, suffix[i], depth, ground, label);
						if (label == ifLabeled)
						{
							colorFiles.push_back(filepath + "\\" + fileinfo.name);
							if (label)
							{
								labelFiles.push_back(filepath + "\\" + fileinfo.name + ".txt");
							}
						}
					}
					else if (mode == RGBD)
					{
						depth = true;
						ground = false;
						findFilesfromColor(filepath, filePrefix, suffix[i], depth, ground, label);
						if (depth && (label == ifLabeled))
						{
							colorFiles.push_back(filepath + "\\" + fileinfo.name);
							depthFiles.push_back(filepath + "\\" + filePrefix + "depth." + suffix[i]);
							if (label)
							{
								labelFiles.push_back(filepath + "\\" + fileinfo.name + ".txt");
							}
						}
					}
					else if (mode == RGBDIR)
					{
						depth = true;
						ground = true;
						findFilesfromColor(filepath, filePrefix, suffix[i], depth, ground, label);
						if (depth && ground && (label == ifLabeled))
						{
							colorFiles.push_back(filepath + "\\" + fileinfo.name);
							depthFiles.push_back(filepath + "\\" + filePrefix + "depth." + suffix[i]);
							IRFiles.push_back(filepath + "\\" + filePrefix + "rightIR." + suffix[i]);
							//if (label)
							//{
							//	labelFiles.push_back(filepath + "\\" + fileinfo.name + ".txt");
							//}
						}
					}

				} while (_findnext(hFile, &fileinfo) == 0);
				_findclose(hFile);
			}
		}
		fileVolume = colorFiles.size();
		if (ifLabeled)
		{
			//std::cout << colorFiles[i] << std::endl;
			if (txtGnssLabel.is_open())
			{
				txtGnssLabel.close();
			}
			txtGnssLabel.open(filepath + "\\of.txt", ios::in);
			if (txtGnssLabel.is_open())
			{
				readTxtGnssLabel(); // 1 - based frame，读取到frames和vertexes
			}
		}
	}

	filePointer = 0;
	
	frames.clear();
	vertexes.clear();


}

void PicGnssFile::findFilesfromColor(string path, string prefix, string suffix, bool& depth, bool&ground, bool& label)
{
	if (depth)
	{
		fstream fileTry(path + "\\" + prefix + "depth." + suffix);
		if (!fileTry)
		{
			depth = false;
		}
		fileTry.close();
	}
	if (ground)
	{
		fstream fileTry(path + "\\" + prefix + "rightIR." + suffix);
		if (!fileTry)
		{
			ground = false;
		}
		fileTry.close();
	}
	//if (label)
	//{
	//	fstream fileTry(path + "\\" + prefix + "color." + suffix + ".txt");
	//	if (!fileTry)
	//	{
	//		label = false;
	//	}
	//	fileTry.close();
	//}
	
}

bool PicGnssFile::doMain()
{
	if (filePointer<fileVolume)
	{		
		colorImg = cv::imread(colorFiles[filePointer], cv::IMREAD_COLOR);
		//imshow("inColor", colorImg);
		if (!latitude.empty() && !longitude.empty())
		{
			latitudeValue = latitude[filePointer];
			longitudeValue = longitude[filePointer];
		}
		if (!posLabel.empty())
		{
			posLabelValue = posLabel[filePointer];
		}
		if (!bestMatch.empty())
		{
			bestMatchValue = bestMatch[filePointer];
		}
		if (mode == PicGnssFile::RGBDIR || mode == PicGnssFile::RGBD)
		{
			depthImg = cv::imread(depthFiles[filePointer], cv::IMREAD_GRAYSCALE);
			imshow("inDepth", depthImg);
		}			
		else
		{
			depthImg = cv::Mat();
		}
		if (mode == PicGnssFile::RGBDIR)
		{
			IRImg = cv::imread(IRFiles[filePointer], cv::IMREAD_GRAYSCALE);
			imshow("inIR", IRImg);
		}			
		else
		{
			IRImg = cv::Mat();
		}
		filePointer++;
		return true;
	}
	else
	{
		return false;
	}
	cv::waitKey(1);
}

cv::Size PicGnssFile::getImgSize()
{
	cv::Mat tmp = cv::imread(colorFiles[filePointer-1]);
	return cv::Size((tmp).cols, tmp.rows);
}


std::vector<std::string> splitWithStl(const std::string &str, const std::string &pattern)
{
	std::vector<std::string> resVec;

	if ("" == str)
	{
		return resVec;
	}
	//方便截取最后一段数据
	std::string strs = str + pattern;

	size_t pos = strs.find(pattern);
	size_t size = strs.size();

	while (pos != std::string::npos)
	{
		std::string x = strs.substr(0, pos);
		resVec.push_back(x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(pattern);
	}

	return resVec;
}

bool PicGnssFile::readTxtGnssLabel()
{
	for (size_t i = 0; i < fileVolume; i++)
	{
		string str;
		getline(txtGnssLabel, str);
		std::vector<std::string> splitStr = splitWithStl(str, "\t");
		assert(splitStr.size() >= 3);
		latitude.push_back(atof(splitStr[1].data())); //string->char*->double
		longitude.push_back(atof(splitStr[2].data()));
		if (splitStr.size() == 6)// idx lat lon time satNum label
		{
			posLabel.push_back(atoi(splitStr[5].data()));
		}
		else if (splitStr.size() == 4)// idx lat lon label
		{
			posLabel.push_back(atoi(splitStr[3].data()));
		}
		else if (splitStr.size() == 7)
		{
			posLabel.push_back(atoi(splitStr[5].data()));
			bestMatch.push_back(atoi(splitStr[6].data()));
		}
		else// idx lat lon 
		{
			posLabel.push_back(0);
		}
		
	}

	return true;
}

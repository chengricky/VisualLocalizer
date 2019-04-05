#include "VisualLocalization.h"
#include "tools\Timer.h"
#include <direct.h>
#include <cmath>

using namespace std;

// 返回时间戳格式为 yyyy-mm-dd_hh-mm-ss
std::string getTimeStamp()
{
	time_t timep;
	timep = time(NULL); /* get current time */
	struct tm *p;
	p = localtime(&timep); /*转换为struct tm结构的当地时间*/
	stringstream timeStampStream;
	timeStampStream << 1900 + p->tm_year << setw(2) << setfill('0') << 1 + p->tm_mon << setw(2) << setfill('0') << p->tm_mday << "_";
	timeStampStream << p->tm_hour << "-" << p->tm_min << "-" << p->tm_sec;
	return timeStampStream.str();
}

bool cmpPair(const std::pair<double, int>& a, const std::pair<double, int>& b)
{
	return(a.first < b.first);
}

VisualLocalization::VisualLocalization(GlobalConfig& config): distanceType(config.distanceType)
{ 	
	if (!config.getValid())
	{
		throw invalid_argument("Configuration is invalid!");
	}
	// 针对读入的feature进行匹配 -- networkflow和bruteforce
	if (config.backendType=="NetworkFlow")
	{
		backendType = BackendType::NetworkFlowTopology;
	}
	else if (config.backendType == "BruteForce")
	{
		backendType = BackendType::BruteForce;
	}
	// 针对读入的image进行匹配 -- ordinary
	else
	{
		backendType = BackendType::Ordinary;
	}
	costHidden = config.costHidden;
	descriptorbase = new Descriptorbase(config);
	descriptorquery = new DescriptorQuery(config);
	std::cout << "VisualLocalization is built! OK" << std::endl;
};

VisualLocalization::~VisualLocalization()
{
	if (descriptorbase != nullptr)
	{
		delete descriptorbase; 
	}
	if (descriptorquery !=nullptr)
	{
		delete descriptorquery;
	}
	if (netFlow!=nullptr)
	{
		delete netFlow;
	}
};

bool VisualLocalization::featureLocalize(const bool &outPR)
{
	// 输出CS GIST LDB BoW GPS特征匹配误差的文件流
	string timeStamp = getTimeStamp();
	// 创建时间戳目录,保存匹配结果
	string content = descriptorquery->pathsTest[0] + "\\" + timeStamp;
	_mkdir(content.c_str());
	// 输出的文件流
	std::fstream fout;
	if (outPR)
	{
		fout.open(content + "\\ofout.txt",ios::out);
		assert(fout.is_open());
	}

	if (backendType == BackendType::NetworkFlowTopology)
	{
		int dims = descriptorbase->xCNNRec.cols;
		netFlow = new NetworkFlow(descriptorbase->xCNNRec, distanceType);
		netFlow->buildGraph(descriptorquery->getCNNQuery(), 4, costHidden, 1);
		cout << "Time of once calculation of cos distance:" << NetworkFlow::timerRet << std::endl;
		netFlow->setParaofGraph(1);
		netFlow->solve(fout, 0);
		netFlow->purge();

		getchar();
	}
	else if (backendType == BackendType::BruteForce)
	{
		std::cout << descriptorquery->getCNNQuery().rows << std::endl;
		std::cout << (descriptorbase->xCNNRec).rows << std::endl;
		for (size_t i = 0; i < descriptorquery->getCNNQuery().rows; i++)
		{
			fout << i << "\t";
			std::vector<std::pair<double, int>> distance;
			for (size_t j = 0; j < (descriptorbase->xCNNRec).rows; j++)
			{
				distance.push_back(std::pair<double, int>(NetworkFlow::cosineDistanceCost(descriptorquery->getCNNQuery().row(i), (descriptorbase->xCNNRec).row(j)), j));
			}
			std::nth_element(distance.begin(), distance.begin(), distance.end(), cmpPair);
			fout << distance[0].second << std::endl;
		}
		return true;
	}
	else
	{
		std::cerr << "ERROR BACKEND TYPE!" << std::endl;
	}
	return true;
}



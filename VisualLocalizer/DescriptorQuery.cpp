#include "DescriptorQuery.h"
#include "tools\list_dir.h"
#include <direct.h>

using namespace std;
using namespace cv;

DescriptorQuery::DescriptorQuery(GlobalConfig& config)
{
	pathsTest.push_back(config.pathTest);

	auto files = listDir(pathsTest[0]);
	std::cout << "Query file number: "<<files.size() << std::endl;
	int loopi = 0;
	for each (auto file in files)
	{
		//if (loopi==2)
		//{
		//	loopi = 0;
		//	continue;
		//}
		std::vector<unsigned char> dim;
		loadFeatureFromFile(file, dim);
		cv::Mat mat = cv::Mat(dim);
		cv::Mat matt = mat.t();
		xCNNQuery.push_back(matt);
		loopi++;
	}
	std::cout << "DescriptorQuery file reading OK!" << std::endl;


	std::cout << "DescriptorQuery is built!" << std::endl;
}

DescriptorQuery::~DescriptorQuery()
{
	if (picsTest!=nullptr)
	{
		delete picsTest;
	}
}

void DescriptorQuery::loadFeatureFromFile(const std::string &filename, std::vector<unsigned char>& dim) {
	// Timer timer;
	// timer.start();
	std::ifstream in(filename.c_str());
	if (!in) {
		printf("[ERROR][Descriptorbase] Feature %s cannot be loaded\n",
			filename.c_str());
		getchar();
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "Reading Query: " << filename << " ..." << std::endl;
	}
	//std::string path;
	//in >> path;

	while (!in.eof()) {
		//unsigned short value;
		//in >> value;
		//dim.push_back(value);
		float value;
		in >> value;
		dim.push_back((unsigned short)value);
	}
	in.close();
	dim.shrink_to_fit();
	// timer.stop();
	// cout << "Feature loading time: ";
	// timer.print_elapsed_time(TimeExt::MSec);
	//binarize();
}
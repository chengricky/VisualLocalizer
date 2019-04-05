#include "database.h"
#include "tools\Timer.h"
#include "tools\list_dir.h"
#include "direct.h"

Descriptorbase::Descriptorbase(GlobalConfig& config)
{
	std::vector<std::string> pathsRec;
	pathsRec.push_back(config.pathRec);

	int loopi = 0;
	auto files = listDir(pathsRec[0]);
	for each (auto file in files)
	{
		//if (loopi<162)
		//{
		//	loopi++;
		//	continue;
		//}
		std::vector<unsigned char> dim;
		loadFeatureFromFile(file, dim);
		cv::Mat mat = cv::Mat(dim);
		cv::Mat matt = mat.t();
		xCNNRec.push_back(matt);			
	}
}

void Descriptorbase::loadFeatureFromFile(const std::string &filename, std::vector<unsigned char>& dim) {
	// Timer timer;
	// timer.start();
	std::ifstream in(filename.c_str());
	if (!in) {
		printf("[ERROR][Descriptorbase] Feature %s cannot be loaded\n",
			filename.c_str());
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "Reading Database: " << filename << " ..."<<std::endl;
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
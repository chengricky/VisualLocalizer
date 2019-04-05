#include "NetworkFlow.h"
#include <iomanip>
#include "..\tools\timer.h"


NetworkFlow::NetworkFlow(const cv::Mat& database, int disType) : d(database.rows), graph(nullptr)
{
	if (disType==1)
	{
		pd = &NetworkFlow::cosineDistanceCost;
	}
	else
	{
		pd = &NetworkFlow::hammingDistanceCost;
	}
		
	//if (d<=k)
	//{
	//	throw std::domain_error("database is smaller than k(default=4), invalid!");
	//}
	database.copyTo(this->database);//深拷贝	
	dimDescriptor = database.cols;

}

bool NetworkFlow::buildGraph(const cv::Mat& query, int k , double w, int f)
{
	if (!query.empty())
	{
		this->query.push_back(query);
	}
	else
	{
		this->query = query.clone();//深拷贝	
	}
	q = query.rows;

	// 构建图的节点数目和边数目
	arc_capacity = 4 * d + 4 * d * k * (q - 1) - 2 * k * (k - 1) * (q-1);
	std::cout << arc_capacity << std::endl;
	if (graph != nullptr)
	{
		delete graph;
	}
	graph = new util::ReverseArcStaticGraph<>(2 * q*d + 2, arc_capacity);


	// 构建连接source的边
	for (size_t i = 1; i <= 2 * d; i++)
	{
		graph->AddArc(0, i);
		weights.push_back(0);
		tails.push_back(0);
		heads.push_back(i);
		if (i % 2==1)//connect to a matching node
		{
			capacitys.push_back(1);
		}
		else//connect to a hidden node
		{
			capacitys.push_back(f + 1);
		}
	}
	// 构建中间的边, 列号奇数是matching nodes，偶数是hidden nodes
	for (size_t h = 1; h < q; h++) // 遍历起点行（query）
	{
		for (size_t i = 1; i <= 2 * d; i++) // 遍历起点列（database）//i=1?
		{
			for (size_t j = (i % 2 == 0) ? i - 1 : i; j < std::min(((i % 2 == 0) ? i - 1 : i) + (k) * 2, (size_t)(2*d+1)); j++) //遍历终点列（database）
			{
				graph->AddArc(i + 2 * d*(h - 1), j + 2 * d*(h));
				tails.push_back(i + 2 * d*(h - 1));
				heads.push_back(j + 2 * d*(h));
				if (j % 2 == 1)// connect to matching node
				{
					//double c = database.row(j / 2).dot((this->query).row(h)) / (cv::norm(database.row(j / 2), cv::NORM_L2)*cv::norm((this->query).row(h), cv::NORM_L2));
					weights.push_back((*pd)(database.row(j / 2), (this->query).row(h)));
					if (i % 2 == 1)
					{
						capacitys.push_back(1);
					}
					else
					{
						capacitys.push_back(f + 1);
					}
				}
				else// connect to hidden node
				{
					weights.push_back(w);
					capacitys.push_back(f + 1);
				}
			}
		}
	}
	// 构建连接sink的边
	for (size_t i = 1; i <= 2 * d; i++)
	{
		graph->AddArc(i + 2 * d*(q - 1), 2 * q*d + 1);
		tails.push_back(i + 2 * d*(q - 1));
		heads.push_back(2 * q*d + 1);
		weights.push_back(0);
		if (i % 2)
		{
			capacitys.push_back(1);
		}
		else
		{
			capacitys.push_back(f + 1);
		}
	}
	std::vector<int> permutation;
	graph->Build(&permutation);  // A static graph must be Build() before usage.
	util::Permute(permutation, &weights);  // Build() may permute the arc index.
	util::Permute(permutation, &capacitys);  // Build() may permute the arc index.
	util::Permute(permutation, &tails);
	util::Permute(permutation, &heads);

	// 初始化min cost flow对象，在此之前graph结构要弄好
	minCostFlow = new operations_research::GenericMinCostFlow<util::ReverseArcStaticGraph<>>(graph);

	return true;
}

bool NetworkFlow::setParaofGraph(int f)
{
	/* SUPPLY */
	// 创建source节点
	minCostFlow->SetNodeSupply(0, f);
	// 创建sink节点
	minCostFlow->SetNodeSupply(2 * q*d + 1, -f);
	// 创建其他节点
	for (size_t i = 1; i < 2 * q*d + 1; i++)
	{
		minCostFlow->SetNodeSupply(i, 0);
	}
	/* COST & CAPACITY */
	std::cout << "arc_capacity: " << arc_capacity;
	assert(weights.size() == arc_capacity);
	assert(capacitys.size() == arc_capacity);
	for (size_t i = 0; i < arc_capacity; i++)
	{
		minCostFlow->SetArcUnitCost(i, 100*weights[i]);
		minCostFlow->SetArcCapacity(i, capacitys[i]);
	}
	return true;
}

bool NetworkFlow::purge()
{
	weights.clear();
	capacitys.clear();
	heads.clear();
	tails.clear();
	return true;
}

bool NetworkFlow::solve(std::fstream& fout, int offset)
{
	// Find the min cost flow.

	bool solveStatus = minCostFlow->Solve();
	if (solveStatus == true)
	{
		long optimalCost = minCostFlow->GetOptimalCost();
		std::cout << "Minimum cost: " << optimalCost;
		std::cout << std::endl;
		//
		std::cout << "Edge   Flow / Capacity  Cost"<< std::endl;

		for (int i = 0; i < arc_capacity; ++i)
		{
			if (minCostFlow->Flow(i)==0)
			{
				continue;
			}
			long cost = minCostFlow->Flow(i) * minCostFlow->UnitCost(i);

			if (tails[i] % (2 * d) % 2)
			{
				fout << tails[i] / (2 * d) + offset << " " << tails[i] % (2 * d) / 2 << std::endl;
				std::cout << tails[i] / (2 * d) + offset << " " << tails[i] % (2 * d) / 2 << std::endl;
				//std::cout << "query: " << tails[i] / (2 * d) << " database: " << tails[i] % (2 * d) / 2 << " -> ";
				//std::cout << heads[i] << "   ";
				//std::cout << minCostFlow->Flow(i) << " / ";
				//std::cout << minCostFlow->Capacity(i) << "       ";
				//std::cout << cost << std::endl;
			}

		}
		std::cout << "OUTPUT txt file OK!" << std::endl;
	}
	return solveStatus;
}

/**
* @brief This method computes the Hamming distance between two binary
* descriptors
* @param desc1 First descriptor
* @param desc2 Second descriptor
* @return Hamming distance between the two descriptors
*/
double NetworkFlow::hammingDistanceCost(cv::Mat desc1, cv::Mat desc2)
{
	int distance = 0;

	if (desc1.rows != desc2.rows || desc1.cols != desc2.cols || desc1.rows != 1 || desc2.rows != 1) {

		std::cout << "The dimension of the descriptors is different." << std::endl;
		return -1;

	}

	for (int i = 0; i < desc1.cols; i++) {
		distance += (*(desc1.ptr<unsigned char>(0) + i)) ^ (*(desc2.ptr<unsigned char>(0) + i));
	}

	return distance;
}

double NetworkFlow::cosineDistanceCost(cv::Mat desc1, cv::Mat desc2)
{

	if (desc1.rows != desc2.rows || desc1.cols != desc2.cols || desc1.rows != 1 || desc2.rows != 1) {

		std::cout << "The dimension of the descriptors is different." << std::endl;
		return -1;

	}
	Timer timer;
	timer.start();
	double cos = desc1.dot(desc2) / (cv::norm(desc1, cv::NORM_L2)*cv::norm(desc2, cv::NORM_L2));
	timer.stop();
	timerRet = timer.get_elapsed_ns().count();


	return (1.0/cos);
}

long long NetworkFlow::timerRet;


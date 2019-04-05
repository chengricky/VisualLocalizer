#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include "..\Header.h"
#include <ortools\graph\min_cost_flow.h>
#include <chrono>
#include <fstream>

//#pragma comment(lib,"ortools.lib")

class NetworkFlow
{
public:
	// construct data associataion graph - 构建
	NetworkFlow(const cv::Mat& database, int disType);
	virtual ~NetworkFlow() {};

	bool buildGraph(const cv::Mat& query, int k = 4, double w = 3, int f = 3);// default: k(4), w(3), f(3),
	bool setParaofGraph( int f = 3);
	bool purge();
	bool solve(std::fstream&, int);

	static double hammingDistanceCost(cv::Mat desc1, cv::Mat desc2);
	static double cosineDistanceCost(cv::Mat desc1, cv::Mat desc2);

	static long long timerRet;

private:
	int d; // number of database
	int q; // number of query images
	//int f; // flow generated from source node
	//int k; //
	int dimDescriptor;
	//double w; // weight of edges connected to hidden nodes 
	int distanceChoice;
	int arc_capacity;

	std::vector<double> weights;
	std::vector<int> capacitys;
	std::vector<unsigned long> tails;
	std::vector<unsigned long> heads;

	// 每一行是一个descriptor，行数代表database数目
	cv::Mat database;
	// 每一行是一个descriptor，行数代表query数目，每次读入一个都追加
	cv::Mat query;

	// GRAPH - Define sources, destinations, capacities, and unit costs between each pair.
	util::ReverseArcStaticGraph<>* graph; //operations_research::
	// a GenericMinCostFlow solver.
	operations_research::GenericMinCostFlow<util::ReverseArcStaticGraph<>>* minCostFlow;

	typedef double (*pDistance)(cv::Mat, cv::Mat);
	pDistance pd;
};
///////////////////////////////////////////////////////////////
// GraphSingleton.cpp - Graph Library                        //
// Ver 1.0                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Dell E6510, Windows 7                        //
// Application: CSE687 - OOD, Pr#1, Spring 2013              //
// Author:      Matt Synborski                               //
//              matthewsynborski@gmail.com                   //
///////////////////////////////////////////////////////////////
//                                                           //
// GraphSingleton shall(1) provide the ability to access     //
// a graph instance via the Singleton pattern.               //
//                                                           //
///////////////////////////////////////////////////////////////

#include "GraphXml.h"
#include "GraphSingleton.h"

#include <iostream>

typedef GraphLib::Graph<std::string, std::string> graph;
typedef GraphLib::Vertex<std::string, std::string> vertex;
typedef GraphLib::Display<std::string, std::string> display;
typedef GraphXml<std::string, std::string> graphXml;

bool GraphSingleton::instanceFlag = false;
GraphSingleton* GraphSingleton::single = NULL;

#ifdef TEST_GRAPHSINGLETON

int main()
{
	std::cout << "Testing graphSingleton:\n";
	std::cout << "=======================\n";
	std::cout << "GraphSingleton shall(1) provide the ability to access     \n";
	std::cout << "a graph instance via the Singleton pattern.               \n\n";
	std::cout << " Creating two singleton graph instances: 1 and 2\n ================================================\n\n";
	GraphSingleton *sc1,*sc2;
	sc1 = GraphSingleton::getInstance();
	vertex vv1("test1",-1);
	vertex vv2("test2",-1);
	std::cout << " Adding a vertex to instance 1\n ================================================\n\n";
	sc1->addVertex(vv1);

	std::cout << " Adding a vertex to instance 2\n ================================================\n\n";
	sc2 = GraphSingleton::getInstance();
	sc2->addVertex(vv2);
	graph gg;

	gg = sc2->getGraph();
	std::cout << " Showing the complete graph via instance 2\n ================================================\n\n";
	display::show(gg);

	gg = sc1->getGraph();
	std::cout << "\n\n Showing the complete graph via instance 1\n ================================================\n\n";
	display::show(gg);

	getchar();
	return 0;
}

#endif
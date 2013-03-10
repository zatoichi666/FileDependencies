///////////////////////////////////////////////////////////////
// GraphSingleton.h - Graph Library                          //
// Ver 1.0                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Dell E6510, Windows 7                        //
// Application: CSE687 - OOD, Pr#2, Spring 2013              //
// Author:      Matt Synborski                               //
//              matthewsynborski@gmail.com                   //
///////////////////////////////////////////////////////////////
//                                                           //
// GraphSingleton shall(1) provide the ability to access     //
// a graph instance via the Singleton pattern.               //
//                                                           //
///////////////////////////////////////////////////////////////

#ifndef GRAPHSING_H
#define GRAPHSING_H

#include <iostream>
#include "Graph.h"
#include "GraphXml.h"

using namespace GraphLib;

typedef Graph<node, std::string> graph;
typedef Vertex<node, std::string> vertex;

class GraphSingleton : public graph  // implements GraphSingleton shall(1)
{
protected:
	static bool instanceFlag;
	static GraphSingleton* single;
private:
	GraphSingleton()
	{
		//private constructor
		instanceFlag = false;
		single = NULL;
	}
public:
	static GraphSingleton* getInstance()
	{
		if(! instanceFlag)
		{
			single = new GraphSingleton();
			instanceFlag = true;
			return single;
		}
		else
		{
			return single;
		}
	}
	graph getGraph()
	{
		return *single;
	}
	~GraphSingleton()
	{
		instanceFlag = false;
	}

	std::vector<size_t> GraphSingleton::graphSearchVertex(std::string value)
	{
		std::vector<size_t> idVector;
		size_t idVert;
		std::cout << "Searching the graph for vertices containing: " << value << "\n";

		for(auto& vert : single->getGraph())
		{
			if (vert.value() == value)
			{
				idVert = vert.id();
				std::cout << "Found vertex with id: " << vert.id() << " and value: " << vert.value().payload << "\n";
				idVector.push_back(vert.id());
			}
		}
		return idVector;
	}

	void GraphSingleton::graphSearchEdge(std::string value)
	{
		std::vector<size_t> idVector;
		graph g = single->getGraph();
		graph::iterator iter = g.begin();

		std::cout << "Searching the graph for edges containing: '" << value << "'\n";
		while(iter != g.end())
		{
			vertex v = *iter;
			std::string parent;
			parent = v.value().payload;
			for(size_t i=0; i<v.size(); ++i)
			{
				vertex::Edge edge = v[i];
				std::string child = g[edge.first].value().payload;
				std::string relationship = (edge.second);
				if (relationship == value)
				{
					std::cout << "Found edge with value: " << relationship << " to: " << g[edge.first].value().payload << " from: " << v.value().payload << "\n";
				}
			}
			++iter;
		}
	}

	size_t GraphSingleton::addTypeToGraph(std::string vertName)
	{
		bool foundVert = false;
		size_t idVert;
		for(auto& vert : single->getGraph())
		{
			if (vert.value() == vertName)
			{
				foundVert = true;
				idVert = vert.id();
			}
		}
		if (foundVert == false)
		{
			vertex vv1(vertName,-1);
			std::cout << "  Can't find " << vertName << " Adding it to graph\n";
			single->addVertex(vv1);
			idVert = vv1.id();
		}
		return idVert;
	}

	void GraphSingleton::addRelationshipToGraph(std::string parentName, std::string childName, std::string relationship_s)
	{
		GraphSingleton *s;
		s = GraphSingleton::getInstance();
		graph g = s->getGraph();

		bool foundParent = false;
		size_t idParent;
		bool foundChild = false;
		size_t idChild;

		for(auto& vert : s->getGraph())
		{
			if (vert.value() == parentName)
			{
				foundParent = true;
				idParent = vert.id();
			}
		}
		if (foundParent == false)
		{
			idParent = s->addTypeToGraph(parentName);
		}

		for(auto& vert : s->getGraph())
		{
			if (vert.value() == childName)
			{
				foundChild = true;
				idChild = vert.id();
			}
		}
		if (foundChild == false)
		{
			
			idChild = s->addTypeToGraph(childName);

		}
		s->addEdge(
			relationship_s, 
			s->getGraph()[s->getGraph().findVertexIndexById(idParent)], 
			s->getGraph()[s->getGraph().findVertexIndexById(idChild)]	);
	}
};

#endif
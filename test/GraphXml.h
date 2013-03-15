//////////////////////////////////////////////////////////////
// GraphXml.h - Graph Library                                //
// Ver 2.0                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Dell E6510, Windows 7                        //
// Application: CSE687 - OOD, Pr#2, Spring 2013              //
// Author:      Matt Synborski                               //
//              matthewsynborski@gmail.com                   //
///////////////////////////////////////////////////////////////
//                                                           //
// GraphXml shall(1) provide the ability to write            //
// graph instances to .Xml files within the file system.     //
//                                                           //
// GraphXml shall(2) provide the ability to read             //
// graph instances to .Xml files within the file system.     //
//                                                           //
// GraphXml shall(3) provide the Tarjan algorithm            //
// on graph instances.                                       //
//                                                           //
// GraphXml shall(4) perform a topological sort on strongly  //
// connected components.                                     //
//                                                           //
// GraphXml shall(5) condense the sorted (4) SCCs into a     //
// new directed graph                                        //
//                                                           //
///////////////////////////////////////////////////////////////

#ifndef GRAPHXML_H
#define GRAPHXML_H

#include <vector>
#include <list>
#include <stack>
#include <sstream>
#include <algorithm>

#include <iomanip>
#include "XmlReader.h"
#include "XmlWriter.h"
#include "Graph.h"
#include <unordered_map>

using namespace GraphLib;

typedef enum
{
	unvisited,
	temporary,
	permanent
} marking;

struct node
{
	std::string payload;
	size_t lowIndex;
	size_t index;
	marking mark;

	std::string c_str()
	{
		std::stringstream ss_lowIndex, ss_index;
		std::string lowIndex_s, index_s;
		ss_lowIndex << lowIndex;
		ss_lowIndex >> lowIndex_s;
		ss_index << index;
		ss_index >> index_s;
		return "Value : " + payload + " lowIndex: " + lowIndex_s + " index: " + index_s;		
	}
	//--------< constructor >----------------------------------------------------------------------
	node::node(const std::string str)
	{
		payload = str;
		lowIndex = SIZE_MAX;
		index = SIZE_MAX;
		mark = unvisited;

	}

	//--------< destructor >-----------------------------------------------------------------------
	//node::~node(void)  allow default compiled destructor


	//--------< equal comparator >-----------------------------------------------------------------
	bool operator== (const node &n1)
	{
		return ((n1.payload == payload) &&
			(n1.lowIndex == lowIndex) &&
			(n1.index == index));
	}

	//--------< not equal comparator >-------------------------------------------------------------
	bool operator!= (const node &n1)
	{
		return !(operator== (n1));
	}
};

template<typename V, typename E> 
class GraphXml : public Graph<V,E> 
{
	typedef Graph<V,E> graph;
	typedef Vertex<V,E> vertex;
	typedef std::pair<std::string,std::string> edge;

private:

	static size_t whichSCC( std::vector<std::vector<vertex>> stronglyConnectedComponents, vertex& vert )
	{
		size_t indexSCC;
		for (size_t i =0;i< stronglyConnectedComponents.size() ; i++)	
			for (size_t j=0;j< stronglyConnectedComponents[i].size(); j++)
				if (stronglyConnectedComponents[i][j].value().payload == vert.value().payload)
					indexSCC = i;

		return indexSCC;
	}
	static void AddVertexToGraph(const std::string& vertValue, const size_t& vert_id, graph &g )
	{
		int match = -1;
		for (auto v:g)	{
			if (v.id() == vert_id)
			{ match = v.id();}
		}
		if (match == -1)
		{
			vertex vv( node(vertValue),vert_id );
			g.addVertex(vv);
		}
	}


public:

	static graph condensedGraph(std::vector<std::vector<vertex>>& stronglyConnectedComponents, graph& fullGraph )
	{
		graph condensedGraph;
		graph::iterator iter = fullGraph.begin();
		while(iter != fullGraph.end())
		{
			vertex v = *iter;
			std::string t1 = v.value().payload;
			size_t indexSCC = whichSCC(stronglyConnectedComponents, v);
			for(size_t i=0; i<v.size(); ++i)
			{
				vertex::Edge edge = v[i];
				AddVertexToGraph(collapseSccIntoString(stronglyConnectedComponents[indexSCC]), indexSCC, condensedGraph);
				size_t indexSCCchild = whichSCC(stronglyConnectedComponents, fullGraph[edge.first]);
				if (indexSCC != indexSCCchild )
				{
					AddVertexToGraph(collapseSccIntoString(stronglyConnectedComponents[indexSCCchild]), indexSCCchild, condensedGraph);
					vertex vertParent = condensedGraph[condensedGraph.findVertexIndexById(indexSCC)];
					vertex vertChild = condensedGraph[condensedGraph.findVertexIndexById(indexSCCchild)];
					bool edgeExist = doesEdgeExistAlready(indexSCC, indexSCCchild, condensedGraph);

					if (!edgeExist)
						condensedGraph.addEdge("relationship", vertParent, vertChild );					
				}
			}						
			++iter;
		}
		return condensedGraph;
	}

	static bool doesEdgeExistAlready(size_t parentId, size_t childId, graph& g)
	{
		std::vector<size_t> idVector;
		graph::iterator iter = g.begin();
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
				if ((v.id() == parentId) && (g[edge.first].id() == childId))
				{
					return true;
				}
			}
			++iter;
		}
		return false;
	}

	static std::string collapseSccIntoString(std::vector<vertex> stronglyConnectedComponent)
	{
		std::string collapsedString;
		for (size_t i=0;i<stronglyConnectedComponent.size();i++)
			collapsedString = collapsedString + stronglyConnectedComponent[i].value().payload + ";";

		return collapsedString;
	}

	static graph readXml(XmlReader rdr) // implements GraphXml shall(2)
	{
		rdr.reset();
		std::string str;	
		graph g;
		size_t vert_id_child, vert_id_parent;
		while (rdr.next())
		{
			std::stringstream vid_child;
			std::stringstream vid_parent;
			str = rdr.tag();

			if (str == "vertex")
			{
				std::string vertValue;
				XmlReader::attribElems attrib = rdr.attributes();
				vid_parent << attrib[0].second;
				vertValue = attrib[1].second;
				vid_parent >> vert_id_parent;
				GraphXml::AddVertexToGraph(vertValue, vert_id_parent, g);
			}
			else if (str == "edge")
			{
				std::string vertValue;
				std::string edgeValue;	
				XmlReader::attribElems attrib = rdr.attributes();
				vid_child << attrib[2].second;
				vertValue = attrib[1].second;
				edgeValue = attrib[0].second;
				vid_child >> vert_id_child;
				GraphXml::AddVertexToGraph(vertValue, vert_id_child, g);

				g.addEdge(edgeValue, g[g.findVertexIndexById(vert_id_parent)], g[g.findVertexIndexById(vert_id_child)] );
			}
		}
		return g;
	}




	static void printPrettyGraph(graph g)
	{

		std::cout << "\nPrinting the graph\n";
		std::cout << "__________________\n";

		graph::iterator iter = g.begin();
		while(iter != g.end())
		{
			vertex v = *iter;
			std::string parent;
			parent = v.value().payload;

			std::cout << std::setw(30) << parent << "\n";
			for(size_t i=0; i<v.size(); ++i)
			{
				vertex::Edge edge = v[i];
				std::string child = g[edge.first].value().payload;
				std::string relationship = (edge.second);

				if (relationship == "variable")
					relationship = "V----variable------>";

				if (relationship == "globalVar")
					relationship = "GV---globalVar----->";

				if (relationship == "globalFun")
					relationship = "GF---globalFun----->";

				if (relationship == "inherits")
					relationship = "<|----inherits------";
				
				if (relationship == "retType")
					relationship = "RT/P---retType----->";

				if (relationship == "param")
					relationship = "RT/P---param------->";

				std::cout << std::setw(30) << "";
				std::cout << std::setw(30) << relationship;
				std::cout << std::setw(30) << child;
				std::cout << "\n";
			}
			++iter;
		}
	}

	static void writeXml(std::string filename, graph g) // implements GraphXml shall(1)
	{
		XmlWriter wtr;
		wtr.addDeclaration();
		wtr.addComment("Matthew Synborski");
		wtr.start("graph");
		graph::iterator iter = g.begin();
		while(iter != g.end())
		{
			XmlWriter vertXml;
			vertXml.start("vertex");
			vertex v = *iter;
			std::string t1;
			t1 =  std::to_string(v.id());
			vertXml.addAttribute("id",t1 );
			t1 = v.value().payload;
			vertXml.addAttribute("value",t1);
			for(size_t i=0; i<v.size(); ++i)
			{
				vertex::Edge edge = v[i];
				XmlWriter edgeXml;
				edgeXml.start("edge /");
				t1 = (edge.second).c_str();
				edgeXml.addAttribute("value",t1);
				t1 = g[edge.first].value().payload.c_str();
				edgeXml.addAttribute("points",t1 );
				t1 = std::to_string(g[edge.first].id());
				edgeXml.addAttribute("id",t1 );
				vertXml.addBody(edgeXml.xml());			
			}
			vertXml.end();
			wtr.addBody(vertXml.xml());
			++iter;
		}
		wtr.end();
		std::ofstream out(filename);
		if(out.good())
		{
			out << wtr.xml().c_str();
			out.close();
		}
	}
};




template<typename V, typename E> 
class TarjanAlgorithm : public Graph<V,E> 
{
	typedef Graph<V,E> graph;
	typedef Vertex<V,E> vertex;
	typedef std::pair<std::string,std::string> edge;
	int index;
	std::vector<vertex> Stk;
	std::vector<std::vector<vertex>> stronglyConnectedComponents;
	graph dg;

	void strongConnect(vertex v)
	{
		size_t nodeVindex = dg.findVertexIndexById(v.id());
		dg[nodeVindex].value().index = index;
		dg[nodeVindex].value().lowIndex = index;
		index++;
		Stk.push_back(dg[nodeVindex]);	
		for(size_t i=0; i<dg[nodeVindex].size(); ++i)
		{
			vertex::Edge edge = dg[nodeVindex][i];
			size_t nodeWindex = edge.first;
			bool ContainsW = false; //search the stack for dg[edge.first]
			for (size_t j=0; j<Stk.size(); j++)
			{
				if (Stk[j].value() == dg[nodeWindex].value())
					ContainsW = true;
			}
			if ( dg[nodeWindex].value().index == SIZE_MAX )
			{
				strongConnect( dg[nodeWindex] );	
				if (dg[nodeVindex].value().lowIndex != SIZE_MAX)	{
					dg[nodeVindex].value().lowIndex = 
						std::min(dg[nodeVindex].value().lowIndex,dg[nodeWindex].value().lowIndex);
				}
			}
			else if (ContainsW)
			{ 				
				if (dg[nodeVindex].value().lowIndex != SIZE_MAX){
					dg[nodeVindex].value().lowIndex = 
						std::min( dg[nodeVindex].value().lowIndex,dg[nodeWindex].value().index); 
				}
			}
		}
		if (dg[nodeVindex].value().lowIndex == dg[nodeVindex].value().index)
		{
			std::vector<vertex> scc;
			node n("z");
			vertex w(n);			
			do
			{		
				w = Stk.back();
				scc.push_back(w);
				Stk.pop_back();
			} while (dg[nodeVindex].value() != w.value());
			stronglyConnectedComponents.push_back(scc);
		}
	}

public:
	std::vector<std::vector<vertex>> getSCC()
	{
		return stronglyConnectedComponents;
	}

	std::vector<std::vector<vertex>> tarjan(graph g)
	{
		stronglyConnectedComponents.clear();
		index = 0;
		Stk.clear();
		dg = g;
		for (auto vert: dg)
		{
			if (vert.value().index == SIZE_MAX)
				strongConnect(vert);			
		}
		return stronglyConnectedComponents;
	}
};

template<typename V, typename E> 
class TopoSort : public Graph<V,E> 
{
	typedef Graph<V,E> graph;
	typedef Vertex<V,E> vertex;

	graph dg;

	//http://en.wikipedia.org/wiki/Topological_sorting
	std::vector<vertex> topoSortList;

	void visit(vertex v)
	{		
		if (v.value().mark == temporary)
		{
			std::cout << "\n\n (!!!) TopoSort: Stopping, not a DAG (!!!) \n";
		}
		else if (v.value().mark == unvisited)
		{
			dg[dg.findVertexIndexById(v.id())].value().mark = temporary;
			for (size_t i=0;i<v.size();i++)
			{
				vertex::Edge edge = v[i];
				vertex child = dg[edge.first];
				visit(child);
			}
			dg[dg.findVertexIndexById(v.id())].value().mark = permanent;
			topoSortList.push_back(v);
		}

	}

	vertex findUnmarkedNode(int& ret)
	{
		ret = 0;
		for (auto vert: dg)
		{
			if (vert.value().mark != permanent)
			{
				ret++;
				return dg[dg.findVertexIndexById(vert.id())];
			}
		}
		//all vertices are marked permanent
		node terminatorNode("terminator");
		vertex terminator(terminatorNode);
		return terminator;
	}

public:
	std::vector<vertex> getTopoSortList()
	{
		return topoSortList;
	}
	void topoSort(graph g)
	{
		dg = g;
		int ret = 0;
		vertex v = findUnmarkedNode(ret);
		while (ret > 0)
		{
			v = findUnmarkedNode(ret);
			if (v.value().payload != "terminator")
				visit(v);
		}
	}

};

#endif
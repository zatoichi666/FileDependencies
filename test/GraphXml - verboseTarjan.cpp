///////////////////////////////////////////////////////////////
// GraphXml.cpp - Graph Library                              //
// Ver 1.0                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Dell E6510, Windows 7                        //
// Application: CSE687 - OOD, Pr#1, Spring 2013              //
// Author:      Matt Synborski                               //
//              matthewsynborski@gmail.com                   //
///////////////////////////////////////////////////////////////
//                                                           //
// GraphXml shall(1) provide the ability to write            //
// graph instances to .Xml files within the file system.     //
//                                                           //
// GraphXml shall(2) provide the ability to read             //
// graph instances from .Xml files within the file system.   //
//                                                           //
///////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

#include "GraphXml.h"
#include "XmlReader.h"
#include "XmlWriter.h"

using namespace GraphLib;

typedef Graph<node, std::string> graph;
typedef Vertex<node, std::string> vertex;
typedef Display<node, std::string> display;
typedef GraphXml<node, std::string> graphXml;
typedef TarjanAlgorithm<node, std::string> tarjanAlgorithm;

template<typename V, typename E>
void showVert(Vertex<V,E>& v)
{
	std::cout << "\n  " << v.id();
}


#ifdef TEST_GRAPHXML

int main()
{
	std::cout << "\nTesting GraphXml Library";
	std::cout << "\n=======================\n\n";
	std::cout << "Testing GraphXml shall(1) provide the ability to write \n";
	std::cout << "graph instances to .Xml files within the file system.  \n\n";        
	std::cout << " Creating a new graph";
	std::cout << "\n Testing XmlReader";
	std::cout << "\n ===================\n";
	graph fGraph;


	//http://upload.wikimedia.org/wikipedia/commons/thumb/5/5c/Scc.png/220px-Scc.png
	node n("a");
	vertex a(n);
	n.payload = "b";
	vertex b(n);
	n.payload = "c";
	vertex c(n);
	n.payload = "d";
	vertex d(n);
	n.payload = "e";
	vertex e(n);
	n.payload = "f";
	vertex f(n);
	n.payload = "g";
	vertex g(n);
	n.payload = "h";
	vertex h(n);

	fGraph.addVertex(a);
	fGraph.addVertex(b);
	fGraph.addVertex(c);
	fGraph.addVertex(d);
	fGraph.addVertex(e);
	fGraph.addVertex(f);
	fGraph.addVertex(g);
	fGraph.addVertex(h);


	fGraph.addEdge("e1",a,b);
	fGraph.addEdge("e2",b,c);
	fGraph.addEdge("e10",c,d);
	fGraph.addEdge("e12",d,h);
	fGraph.addEdge("e13",h,g);
	fGraph.addEdge("e8",g,f);

	fGraph.addEdge("e3",e,a);
	fGraph.addEdge("e4",b,f);
	fGraph.addEdge("e5",e,f);
	fGraph.addEdge("e6",b,e);
	fGraph.addEdge("e7",c,g);
	
	fGraph.addEdge("e9",f,g);

	fGraph.addEdge("e11",d,c);



	fGraph.addEdge("e14",h,d);


	display::show(fGraph);

	graphXml::writeXml("test.xml",fGraph);

	std::cout << "\n\n Testing GraphXml shall(2) provide the ability to read \n";
	std::cout << " graph instances from .Xml files within the file system. ";       
	std::cout << "\n\n Reading the .xml file from the file system.\n";
	std::cout << "----------------------------------\n";
	std::string line;
	std::ifstream xmlFile ("test.xml");
	std::string infile;
	if (xmlFile.is_open())
	{
		while ( xmlFile.good() )
		{
			std::getline(xmlFile,line);
			infile += line;
		}
		xmlFile.close();
	}
	std::cout << infile << std::endl;

	graph gGraph;
	XmlReader rdr(infile);
	gGraph = graphXml::readXml(rdr);
	display::show(gGraph);
	tarjanAlgorithm g2;

	g2.tarjan(gGraph);	

	getchar();
	return 0;
}

#endif
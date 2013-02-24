///////////////////////////////////////////////////////////////
// Graph.cpp - Graph Library                                 //
// Ver 1.1                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Lenova ThinkPad E420s, Windows 7             //
// Application: Help for CSE687 - OOD, Pr#1, Spring 2013     //
// Author:      Jim Fawcett, CST 4-187, Syracuse University  //
//              (315) 443-3948, jfawcett@twcny.rr.com        //
///////////////////////////////////////////////////////////////

#include <iostream>
#include "Graph.h"
using namespace GraphLib;

typedef Graph<std::string, std::string> graph;
typedef Vertex<std::string, std::string> vertex;
typedef Display<std::string, std::string> display;

template<typename V, typename E>
void showVert(Vertex<V,E>& v)
{
  std::cout << "\n  " << v.id();
}
#ifdef TEST_GRAPH

int main()
{
  std::cout << "\n  Testing Graph Library";
  std::cout << "\n =======================\n";
  try
  {
    std::cout << "\n  Constructing Graph instance";
    std::cout << "\n -----------------------------";
    graph g;
    vertex v1("v1");
    vertex v2("v2");
    vertex v3("v3");
    vertex v4("v4");
    vertex v5("v5", 50);
    g.addVertex(v2);
    g.addVertex(v1);
    g.addVertex(v3);
    g.addVertex(v4);
    g.addVertex(v5);
    g.addEdge("e1",v1,v2);
    g.addEdge("e2",v1,v3);
    g.addEdge("e3",v2,v3);
    g.addEdge("e4",v4,v3);
    g.addEdge("e5",v5,v2);

    display::show(g);
    std::cout << "\n";

    std::cout << "\n  Making copy of instance";
    std::cout << "\n -------------------------";
    graph gcopy = g;
    display::show(gcopy);
    std::cout << "\n";

    std::cout << "\n  Modifying copy's values";
    std::cout << "\n -------------------------";
    for(auto& v : gcopy)
      v.value() += "copy";

    display::show(gcopy);
    std::cout << "\n";

    std::cout << "\n  Assigning original instance to copy";
    std::cout << "\n -------------------------------------";
    gcopy = g;
    display::show(gcopy);
    std::cout << "\n";

    std::cout << "\n  Vertices with no Parents:";
    std::cout << "\n ---------------------------";

    std::vector< vertex > verts = display::vertsWithNoParents(g);
    std::cout << "\n  ";
    for(size_t i=0; i<verts.size(); ++i)
      std::cout << verts[i].value().c_str() << " ";
    std::cout << "\n";

    std::cout << "\n  Testing Depth First Search";
    std::cout << "\n ----------------------------";
    for(auto& vert : g)
    {
      std::cout << "\n  starting at id " << vert.id();
      g.dfs(vert, showVert);
    }
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }
  std::cout << "\n\n";
  return 0;
}

#endif


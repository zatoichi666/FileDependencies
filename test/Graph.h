#ifndef GRAPH_H
#define GRAPH_H
///////////////////////////////////////////////////////////////
// Graph.h - Graph Library                                   //
// Ver 1.2                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Lenova ThinkPad E420s, Windows 7             //
// Application: Help for CSE687 - OOD, Pr#1, Spring 2013     //
// Author:      Jim Fawcett, CST 4-187, Syracuse University  //
//              (315) 443-3948, jfawcett@twcny.rr.com        //
///////////////////////////////////////////////////////////////
/*
   Package Operations:
   ===================
   A graph is a collection of vertices, connected by edges.  Each
   vertex holds some kind of information held in an instance of
   a type V and an integer id, which should be unique with a given
   graph.  Each edge connects a parent vertex with a child
   vertex and it too holds information in an instance of a type E.

   This package provides two classes that support creation of
   instances of a graph: Vertex<V,E> and Graph<V,E>.  These are
   template-based with parameters V and E, as described above.

   The graph holds a composed collection of vertices.  Each 
   vertex may hold one or more edges.  The edge is implemented
   as an instance of std::pair<int,E> where the first parameter
   is the id of the child vertex and the second parameter is
   an instance of the edge information type E.

   Note:
   It is important that vertex ids be unique.  If you set any of
   them with the second argument of the Vertex constructor or with
   the id() function it is up to you to ensure uniqueness.  If you
   don't explicitly set any ids then the constructor insures that
   vertices get sequential ids in the order constructed.

   Maintanence Information:
   ========================
   Required files:
   ---------------
     Graph.h, Graph.cpp

   Build Process:
   --------------
   Using Visual Studio Command Prompt:
     devenv Graph.sln /rebuild debug

   Revision History:
   -----------------
   - ver 1.2 : changed id() return to reference so can be changed
   - ver 1.1 : added prologue comments, compile test directive
   - ver 1.0 : first release

 */
/*
 * To Do:
 * - create graph instance from XML
 * - create XML representation from graph instance
 * - Strong Components, Topological Sorting
 * - Find disconnected graph partitions
 */

#include <vector>
#include <unordered_map>

namespace GraphLib 
{
  ///////////////////////////////////////////////////////////////
  // Vertex class

  template<typename V, typename E> 
  class Vertex
  {
  public:
    typedef std::pair<int, E> Edge;  // graph index of target vertex, edge type
    typename typedef std::vector<Edge>::iterator iterator;
    iterator begin();
    iterator end();
    Vertex(V v, int id=-1);
    void add(Edge& edge);
    // Vertex(const Vertex<V,E>& v);                  compiler generated is correct
    // Vertex<V,E>& operator=(const Vertex<V,E>& v);  compiler generated is correct
    Edge& operator[](size_t i);
    Edge operator[](size_t i) const;
    V& value();
    size_t& id(); 
    size_t size();
    bool& mark();
  private:
    std::vector<Edge> _edges;
    V _v;
    size_t _id;
    static size_t count;
    bool _mark;
  };
  //----< reserve memory for, and initialize, static count >-----
  template<typename V, typename E>
  size_t Vertex<V,E>::count = 0;

  //----< set and return boolean mark, used for traversal >------
  template<typename V, typename E>
  bool& Vertex<V,E>::mark() { return _mark; }

  //----< return iterator pointing to first edge >---------------
  template<typename V, typename E>
  typename Vertex<V,E>::iterator Vertex<V,E>::begin() { return _edges.begin(); }

  //----< return iterator pointing to one past last edge >-------
  template<typename V, typename E>
  typename Vertex<V,E>::iterator Vertex<V,E>::end() { return _edges.end(); }

  //----< construct instance >-----------------------------------
  template<typename V, typename E>
  Vertex<V,E>::Vertex(V v, int id) : _v(v), _id(count++), _mark(false) 
  {
    if(id != -1)
      _id = id;
  }

  //----< add edge to vertex edge collection >-------------------
  template<typename V, typename E>
  void Vertex<V,E>::add(Edge& edge) { _edges.push_back(edge); }

  //----< index non-const vertex's edges >-----------------------
  template<typename V, typename E>
  typename Vertex<V,E>::Edge& Vertex<V,E>::operator[](size_t i) { return _edges[i]; }
  
  //----< index const vertex's edges >---------------------------
  template<typename V, typename E>
  typename Vertex<V,E>::Edge Vertex<V,E>::operator[](size_t i) const { return _edges[i]; }
  
  //----< set and read value of vertex's held type, V >----------
  template<typename V, typename E>
  V& Vertex<V,E>::value() { return _v; }
  
  //----< return vertex's id >-----------------------------------
  template<typename V, typename E>
  size_t& Vertex<V,E>::id() { return _id; }
  
  //----< return number of edges >-------------------------------
  template<typename V, typename E>
  size_t Vertex<V,E>::size() { return _edges.size(); }

  ///////////////////////////////////////////////////////////////
  // Graph class

  template<typename V, typename E>
  class Graph
  {
  public:
    typename typedef std::vector< Vertex<V,E> >::iterator iterator;
    iterator begin();
    iterator end();
    // Graph(const Graph<V,E>& g);                  compiler generated is correct
    // Graph<V,E>& operator=(const Graph<V,E>& g);  compiler generated is correct
    Vertex<V,E>& operator[](size_t i);
    Vertex<V,E> operator[](size_t i) const;
    void addVertex(Vertex<V,E> v);
    void addEdge(E eval, Vertex<V,E>& parent, Vertex<V,E>& child);
    size_t findVertexIndexById(size_t id);
    size_t size();
    void dfs(Vertex<V,E>& v, void(*f)(Vertex<V,E>& v));
  private:
    std::vector< Vertex<V,E> > adj;
    std::unordered_map<size_t, size_t> idMap; // id maps to graph index
    void dfsCore(Vertex<V,E>& v, void(*f)(Vertex<V,E>& v));
  };
  //----< return iterator pointing to first vertex >-------------
  template<typename V, typename E>
  typename Graph<V,E>::iterator Graph<V,E>::begin() { return adj.begin(); }

  //----< return iterator pointing one past last vertex >--------
  template<typename V, typename E>
  typename Graph<V,E>::iterator Graph<V,E>::end() { return adj.end(); }
  
  //----< index non-const graph's vertex collection >------------
  template<typename V, typename E>
  typename Vertex<V,E>& Graph<V,E>::operator[](size_t i) { return adj[i]; }
 
  //----< index const graph's vertex collection >----------------
  template<typename V, typename E>
  typename Vertex<V,E> Graph<V,E>::operator[](size_t i) const { return adj[i]; }
  
  //----< add vertex to graph's vertex collection >--------------
  template<typename V, typename E>
  void Graph<V,E>::addVertex(Vertex<V,E> v) 
  { 
    adj.push_back(v);
    idMap[v.id()] = adj.size() - 1;
  }
  //----< return number of vertices in graph's collection >------
  template<typename V, typename E>
  size_t Graph<V,E>::size() { return adj.size(); }

  //----< return index of vertex with specified id >-------------
  template<typename V, typename E>
  size_t Graph<V,E>::findVertexIndexById(size_t id)
  {
    return idMap[id];
  }
  //----< add edge from specified parent to child vertices >-----
  template<typename V, typename E>
  void Graph<V,E>::addEdge(E eVal, Vertex<V,E>& parent, Vertex<V,E>& child)
  {
    size_t childIndex = findVertexIndexById(child.id());
    if(childIndex == adj.size())
      throw std::exception("no edge child");
    size_t parentIndex = findVertexIndexById(parent.id());
    if(parentIndex == adj.size())
      throw std::exception("no edge parent");
    Vertex<V,E>::Edge e;
    e.first = childIndex;
    e.second = eVal;
    adj[parentIndex].add(e);
  }
  //----< recursive depth first search with action f >-----------
  template<typename V, typename E>
  void Graph<V,E>::dfsCore(Vertex<V,E>& v, void(*f)(Vertex<V,E>& cv))
  {
    f(v);
    v.mark() = true;
    for(auto edge : v)
    {
      if(adj[edge.first].mark() == false)
        dfsCore(adj[edge.first], f);
    }
    for(auto& vert : adj)
    {
      if(vert.mark() == false)
        dfsCore(vert, f);
    }
  }
  //----< depth first search, clears marks for next search >-----
  template<typename V, typename E>
  void Graph<V,E>::dfs(Vertex<V,E>& v, void(*f)(Vertex<V,E>& cv))
  {
    dfsCore(v, f);
    for(auto& vert : adj)
      vert.mark() = false;
  }

  ///////////////////////////////////////////////////////////////
  // Display class

  template<typename V, typename E>
  class Display
  {
  public:
    typedef Graph<V,E> graph;
    typedef Vertex<V,E> vertex;
    typedef std::pair<int,E> edge;

    static std::vector< Vertex<V,E> > vertsWithNoParents(graph& g)
    {
      std::vector<size_t> parentCount;
      graph::iterator iter = g.begin();
      while(iter != g.end())
      {
        parentCount.push_back(0);
        ++iter;
      }
      iter = g.begin();
      while(iter != g.end())
      {
        vertex v = *iter;
        for(size_t i=0; i<v.size(); ++i)
        {
          edge e = v[i];
          parentCount[e.first]++;
        }
        ++iter;
      }
      std::vector<vertex> noParents;
      for(size_t j=0; j<g.size(); ++j)
        if(parentCount[j] == 0)
          noParents.push_back(g[j]);
      return noParents;
    }

    static void show(graph& g)
    {
      graph::iterator iter = g.begin();
      while(iter != g.end())
      {
        vertex v = *iter;
        std::cout << "\n  vertex id = " << v.id() << ", value = " << (v.value()).c_str();
        for(size_t i=0; i<v.size(); ++i)
        {
          vertex::Edge edge = v[i];
          std::cout << "\n    edge points to vertex with id = " << g[edge.first].id();
          std::cout << " and value = " << g[edge.first].value().c_str();
          std::cout << ", edge value = " << (edge.second).c_str();
        }
        ++iter;
      }
    }
  };
}
#endif

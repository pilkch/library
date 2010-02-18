#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <iostream>
#include <fstream>
#include <sstream>

#include <list>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <algorithm>

#include <breathe/game/cAIPathFinder.h>

namespace breathe
{
  namespace ai
  {
    class cEdge;

    class cNode
    {
    public:
      void Sort(); // Sort the edges travelling away from this node in order of cheapest first

      spitfire::math::cVec3 position;
      float fCost;

      std::vector<cEdge*> vEdges;
    };


    class cEdge
    {
    public:
      static bool CostCompare(const cEdge* lhs, const cEdge* rhs);

      cNode* pNodeFrom;
      cNode* pNodeTo;
      float fCost;
    };

    // Sort the edges travelling away from this node in order of cheapest first

    void cNode::Sort()
    {
      std::sort(vEdges.begin(), vEdges.end(), cEdge::CostCompare);
    }

    // Comparison for sorting edges based on cost

    inline bool cEdge::CostCompare(const cEdge* pLHS, const cEdge* pRHS)
    {
      ASSERT(pLHS != nullptr);
      ASSERT(pRHS != nullptr);
      return (pLHS->fCost > pRHS->fCost);
    }


    // NOTE: All nodes must be added first, then edges
    class cGraph
    {
    public:
      size_t AddNode(const spitfire::math::cVec3& position, float fCost);

      // NOTE: The total cost is fCostMultiplier * length(start to end)
      void AddEdge(size_t nodeFrom, size_t nodeTo, float fCostMultiplier);

      void Build(); // Sorts the edges within each node, may do more work later on such as calculating distances and costs

      const cNode& GetNode(size_t i) const { ASSERT(i < nodes.size()); return nodes[i]; }
      const cEdge& GetEdge(size_t i) const { ASSERT(i < edges.size()); return edges[i]; }

    private:
      void Sort(); // Sort the edges in cNode::vEdges

      std::vector<cNode> nodes;
      std::vector<cEdge> edges;
    };

    size_t cGraph::AddNode(const spitfire::math::cVec3& position, float fCost)
    {
      size_t index = nodes.size();

      cNode node;
      node.position = position;
      node.fCost = fCost;
      nodes.push_back(node);

      return index;
    }

    void cGraph::AddEdge(size_t nodeFrom, size_t nodeTo, float fCostMultiplier)
    {
      ASSERT(nodeFrom < nodes.size());
      ASSERT(nodeTo < nodes.size());

      ASSERT(nodeFrom != nodeTo);

      cNode* pNodeFrom = &nodes[nodeFrom];
      cNode* pNodeTo = &nodes[nodeTo];

      cEdge edge;
      edge.pNodeFrom = pNodeFrom;
      edge.pNodeTo = pNodeTo;
      edge.fCost = fCostMultiplier * (pNodeFrom->position - pNodeTo->position).GetLength();

      size_t index = edges.size();
      edges.push_back(edge);

      pNodeFrom->vEdges.push_back(&edges[index]);
    }

    void cGraph::Sort()
    {
      // Now sort the edges within each node
      const size_t n = nodes.size();
      for (size_t i = 0; i < n; i++) {
        nodes[i].Sort();
      }
    }

    void cGraph::Build()
    {
      Sort();
    }


    // Notes:
    // Each edge is in a single direction, between node0 and node1 there could be hundreds of edges going either direction.
    // Each node only knows about the edges heading away from it.
    class cDijkstra
    {
    public:
      void GetLowestCostPath(const cGraph& graph, size_t nodeStart, size_t nodeEnd, std::vector<size_t>& path);

    private:
      void _GetLowestCostPathRecursive(const cGraph& graph, const cNode* pNodeCurrent, const cNode* pNodeEnd, std::vector<size_t>& path);
    };

    void cDijkstra::GetLowestCostPath(const cGraph& graph, size_t nodeStart, size_t nodeEnd, std::vector<size_t>& path)
    {
      path.clear();

      // If we are already at the end node then we have nothing to calculate, so return
      if (nodeStart == nodeEnd) return;


      const cNode* pNodeCurrent = &graph.GetNode(nodeStart);
      const cNode* pNodeEnd = &graph.GetNode(nodeEnd);

      while (pNodeCurrent != pNodeEnd) {
        _GetLowestCostPathRecursive(graph, pNodeCurrent, pNodeEnd, path);
      };
    }

    void cDijkstra::_GetLowestCostPathRecursive(const cGraph& graph, const cNode* pNodeCurrent, const cNode* pNodeEnd, std::vector<size_t>& path)
    {
      // TODO: Do stuff here

      ...

      while (vs.size() > 0) {
        int x = vs.front();//take the node with the shortest distance
        const size_t n = vs.size() - 1;
        for (size_t i = 0; i < n; i++) {
          vs.at(i) = vs.at(i + 1);
        }
        vs.pop_back();
        s.push_back(x);//mark it as visited

        relax(vs, x);//update the distances

        sort(vs); //sort the nodes according to the new distances
      }
    }


    void UnitTest()
    {
      cGraph graph;
      graph.AddNode(const spitfire::math::cVec3& position, 1.0f);
      graph.AddEdge(size_t start, size_t end, 1.0f);

      graph.Build();


      std::vector<size_t> path;

      cDijkstra dijkstra;
      dijkstra.GetLowestCostPath(graph, 0, 4, path);

      ...
    }
  }
}

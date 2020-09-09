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
    // ** cNode

    // Sort the edges travelling away from this node in order of cheapest first
    void cNode::SortEdgesCheapestFirst()
    {
      std::sort(vEdges.begin(), vEdges.end(), cEdge::CostCompare);
    }


    // ** cGraph

    cGraph::~cGraph()
    {
      {
        // Delete all nodes
        const size_t n = nodes.size();
        for (size_t i = 0; i < n; i++) {
          spitfire::SAFE_DELETE(nodes[i]);
        }
      }

      {
        // Delete all edges
        const size_t n = edges.size();
        for (size_t i = 0; i < n; i++) {
          spitfire::SAFE_DELETE(edges[i]);
        }
      }
    }

    size_t cGraph::AddNode(const spitfire::math::cVec3& position, float fCost)
    {
      const size_t index = nodes.size();

      cNode* pNode = new cNode;
      pNode->index = index;
      pNode->position = position;
      pNode->fCost = fCost;
      nodes.push_back(pNode);

      return index;
    }

    void cGraph::AddEdge(size_t nodeFrom, size_t nodeTo, float fCostMultiplier)
    {
      ASSERT(nodeFrom < nodes.size());
      ASSERT(nodeTo < nodes.size());

      ASSERT(nodeFrom != nodeTo);

      cNode* pNodeFrom = nodes[nodeFrom];
      cNode* pNodeTo = nodes[nodeTo];

      cEdge* pEdge = new cEdge;
      pEdge->pNodeFrom = pNodeFrom;
      pEdge->pNodeTo = pNodeTo;
      pEdge->fCost = fCostMultiplier * (pNodeFrom->position - pNodeTo->position).GetLength();

      edges.push_back(pEdge);

      pNodeFrom->vEdges.push_back(pEdge);
    }

    void cGraph::Sort()
    {
      // Now sort the edges within each node
      const size_t n = nodes.size();
      for (size_t i = 0; i < n; i++) {
        nodes[i]->SortEdgesCheapestFirst();
      }
    }

    void cGraph::Optimise()
    {
      Sort();
    }

    const cNode* cGraph::GetClosestNode(const spitfire::math::cVec3& position) const
    {
      float fClosestDistanceSquared = spitfire::math::cINFINITY;
      const cNode* pClosestNode = nullptr;

      const size_t n = nodes.size();
      for (size_t i = 0; i < n; i++) {
        const float fDistanceSquared = (nodes[i]->position - position).GetSquaredLength();
        if (fDistanceSquared < fClosestDistanceSquared) {
          fClosestDistanceSquared = fDistanceSquared;
          pClosestNode = nodes[i];
        }
      }

      return pClosestNode;
    }


    // ** cDijkstra

    void cDijkstra::ResetContext(const cGraph& graph)
    {
      const size_t n = graph.GetNumberOfNodes();

      // Reset distance values to infinity and visited to false for each node
      if ((nodeDistanceFromStart.size() != n) || (nodeVisited.size() != n)) {
        nodeDistanceFromStart.clear();
        nodeVisited.clear();

        // 1. Assign to every node a distance value. Set all nodes to infinity.
        // 2. Mark all nodes as unvisited. Set initial node as current.
        nodeDistanceFromStart.resize(n, spitfire::math::cINFINITY);
        nodeVisited.resize(n, false);
      } else {
        // 1. Assign to every node a distance value. Set all nodes to infinity.
        // 2. Mark all nodes as unvisited. Set initial node as current.
        std::fill(nodeDistanceFromStart.begin(), nodeDistanceFromStart.end(), spitfire::math::cINFINITY);
        std::fill(nodeVisited.begin(), nodeVisited.end(), false);
      }
    }

    void cDijkstra::GetLowestCostPath(const cGraph& graph, const spitfire::math::cVec3& start, const spitfire::math::cVec3& end, std::vector<size_t>& path)
    {
      //std::cout<<"cDijkstra::GetLowestCostPath"<<std::endl;

      path.clear();

      ResetContext(graph);

      const cNode* pNodeCurrent = graph.GetClosestNode(start);
      ASSERT(pNodeCurrent != nullptr);

      const cNode* pNodeEnd= graph.GetClosestNode(end);
      ASSERT(pNodeEnd != nullptr);

      // Set the distance to zero for our initial node
      nodeDistanceFromStart[pNodeCurrent->index] = 0.0f;

      _GetLowestCostPathRecursive(graph, pNodeCurrent, pNodeEnd, path);
    }

    // TODO: Make path vector a vector of const cNode*

    void cDijkstra::GetLowestCostPath(const cGraph& graph, size_t nodeStart, size_t nodeEnd, std::vector<size_t>& path)
    {
      //std::cout<<"cDijkstra::GetLowestCostPath"<<std::endl;

      path.clear();

      ResetContext(graph);

      // Set the distance to zero for our initial node
      nodeDistanceFromStart[nodeStart] = 0.0f;

      const cNode* pNodeCurrent = &graph.GetNode(nodeStart);
      const cNode* pNodeEnd = &graph.GetNode(nodeEnd);
      _GetLowestCostPathRecursive(graph, pNodeCurrent, pNodeEnd, path);
    }

    void cDijkstra::_GetLowestCostPathRecursive(const cGraph& graph, const cNode* pNodeCurrent, const cNode* pNodeEnd, std::vector<size_t>& path)
    {
      //std::cout<<"cDijkstra::_GetLowestCostPathRecursive"<<std::endl;

      ASSERT(pNodeCurrent != nullptr);
      ASSERT(pNodeEnd != nullptr);

      path.push_back(pNodeCurrent->index);

      // If we are already at the end node then we have nothing to calculate, so return
      if (pNodeCurrent == pNodeEnd) return;

      // 3. For current node, consider all its unvisited neighbours and calculate their distance (from the initial node). For example, if current node (A) has distance of 6, and an edge connecting it with another node (B) is 2, the distance to B through A will be 6+2=8. If this distance is less than the previously recorded distance (infinity in the beginning, zero for the initial node), overwrite the distance.
      const size_t n = pNodeCurrent->vEdges.size();
      for (size_t i = 0; i < n; i++) {
        ASSERT(pNodeCurrent->vEdges[i] != nullptr);
        ASSERT(pNodeCurrent->vEdges[i]->pNodeTo != nullptr);

        const size_t indexNextNode = pNodeCurrent->vEdges[i]->pNodeTo->index;
        if (!nodeVisited[indexNextNode]) {
          const float fEdgeCost = pNodeCurrent->vEdges[i]->fCost;
          if (fEdgeCost < nodeDistanceFromStart[indexNextNode]) nodeDistanceFromStart[indexNextNode] = fEdgeCost;
        }
      }

      // 4. When we are done considering all neighbours of the current node, mark it as visited. A visited node will not be checked ever again; its distance recorded now is final and minimal.
      nodeVisited[pNodeCurrent->index] = true;

      // 5. Set the unvisited node with the smallest distance (from the initial node) as the next “current node” and continue from step 3
      bool bIsVisitedAllNodes = true;
      size_t indexOfLowestCostEdge = 0;
      float fLowestCost = spitfire::math::cINFINITY;
      for (size_t i = 0; i < n; i++) {
        const size_t indexNextNode = pNodeCurrent->vEdges[i]->pNodeTo->index;
        if (!nodeVisited[indexNextNode]) {
          bIsVisitedAllNodes = false;
          const float fEdgeCost = pNodeCurrent->vEdges[i]->fCost;
          if (fEdgeCost < fLowestCost) {
            indexOfLowestCostEdge = i;
            fLowestCost = fEdgeCost;
          }
        }
      }

      if (!bIsVisitedAllNodes) {
        pNodeCurrent = pNodeCurrent->vEdges[indexOfLowestCostEdge]->pNodeTo;
        _GetLowestCostPathRecursive(graph, pNodeCurrent, pNodeEnd, path);
      }
    }
  }
}


#ifdef BUILD_SPITFIRE_UNITTEST

#include <spitfire/util/unittest.h>

class cAIPathFinderUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cAIPathFinderUnitTest() :
    cUnitTestBase(TEXT("cAIPathFinderUnitTest"))
  {
  }

  virtual void Test() override;
};

void cAIPathFinderUnitTest::Test()
{
  const spitfire::math::cVec3 nodes[] = {
    spitfire::math::cVec3(1.0f, 3.0f, 0.0f),
    spitfire::math::cVec3(1.0f, 1.0f, 0.0f),
    spitfire::math::cVec3(2.0f, 1.0f, 0.0f),
    spitfire::math::cVec3(2.0f, 2.0f, 0.0f),
    spitfire::math::cVec3(4.0f, 1.0f, 0.0f),
    spitfire::math::cVec3(10.0f, 4.0f, 0.0f),
    spitfire::math::cVec3(4.0f, 3.0f, 0.0f),
    spitfire::math::cVec3(5.0f, 3.0f, 0.0f),
    spitfire::math::cVec3(6.0f, 1.0f, 0.0f)
  };

  const std::pair<size_t, size_t> edges[] = {
    { 0, 1 },
    { 1, 2 },
    { 2, 3 },
    { 2, 4 },
    { 3, 4 },
    { 3, 5 },
    { 4, 6 },
    { 5, 7 },
    { 6, 7 },
    { 4, 8 },
    { 7, 8 },
  };

  breathe::ai::cGraph graph;

  {
    const size_t n = countof(nodes);
    for (size_t i = 0; i < n; i++) {
      graph.AddNode(nodes[i], 1.0f);
    }
  }
  {
    const size_t n = countof(edges);
    for (size_t i = 0; i < n; i++) {
      graph.AddEdge(edges[i].first, edges[i].second, 1.0f);
    }
  }

  graph.Optimise();


  // Test closest distances calculations for this graph
  {
    ASSERT_EQ(0, graph.GetClosestNode(spitfire::math::cVec3(1.0f, 3.0f, 0.0f))->index);
    ASSERT_EQ(0, graph.GetClosestNode(spitfire::math::cVec3(1.0f, 3.1f, 0.0f))->index);
    ASSERT_EQ(0, graph.GetClosestNode(spitfire::math::cVec3(1.1f, 3.0f, 0.0f))->index);

    ASSERT_EQ(1, graph.GetClosestNode(spitfire::math::cVec3(1.0f, 1.0f, 0.0f))->index);
    ASSERT_EQ(1, graph.GetClosestNode(spitfire::math::cVec3(-10.1f, -30.0f, 0.0f))->index);

    ASSERT_EQ(5, graph.GetClosestNode(spitfire::math::cVec3(10.0f, 4.0f, 0.0f))->index);
    ASSERT_EQ(5, graph.GetClosestNode(spitfire::math::cVec3(100.0f, 40.0f, 0.0f))->index);
  }


  std::vector<size_t> path;

  breathe::ai::cDijkstra dijkstra;
  dijkstra.GetLowestCostPath(graph, 2, 7, path);

  std::cout<<"path:";

  const size_t n = path.size();
  for (size_t i = 0; i < n; i++) {
    std::cout<<" "<<path[i];
  }

  std::cout<<std::endl;

  // Check that the path is correct
  ASSERT(path[0] == 2);
  ASSERT(path[1] == 3);
  ASSERT(path[2] == 4);
  ASSERT(path[3] == 6);
  ASSERT(path[4] == 7);
}

cAIPathFinderUnitTest gAIPathFinderUnitTest;

#endif // BUILD_SPITFIRE_UNITTEST

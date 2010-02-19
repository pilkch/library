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
    void cNode::Sort()
    {
      std::sort(vEdges.begin(), vEdges.end(), cEdge::CostCompare);
    }


    // ** cGraph

    size_t cGraph::AddNode(const spitfire::math::cVec3& position, float fCost)
    {
      size_t index = nodes.size();

      cNode node;
      node.index = index;
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


    // ** cDijkstra

    void cDijkstra::GetLowestCostPath(const cGraph& graph, size_t nodeStart, size_t nodeEnd, std::vector<size_t>& path)
    {
      std::cout<<"cDijkstra::_GetLowestCostPathRecursive"<<std::endl;

      path.clear();

      nodeDistanceFromStart.clear();
      nodeVisited.clear();

      const size_t n = graph.GetNumberOfNodes();
      nodeDistanceFromStart.resize(n, spitfire::math::cINFINITY);
      nodeVisited.resize(n, false);

      nodeDistanceFromStart[nodeStart] = 0.0f;


      const cNode* pNodeCurrent = &graph.GetNode(nodeStart);
      const cNode* pNodeEnd = &graph.GetNode(nodeEnd);
      const float fDistanceFromStartNode = 0.0f;
      _GetLowestCostPathRecursive(graph, pNodeCurrent, pNodeEnd, fDistanceFromStartNode, path);
    }

    void cDijkstra::_GetLowestCostPathRecursive(const cGraph& graph, const cNode* pNodeCurrent, const cNode* pNodeEnd, float fDistanceFromStartNode, std::vector<size_t>& path)
    {
      std::cout<<"cDijkstra::_GetLowestCostPathRecursive"<<std::endl;

      ASSERT(pNodeCurrent != nullptr);
      ASSERT(pNodeEnd != nullptr);

      path.push_back(pNodeCurrent->index);

      // If we are already at the end node then we have nothing to calculate, so return
      if (pNodeCurrent == pNodeEnd) return;

      const size_t n = pNodeCurrent->vEdges.size();
      for (size_t i = 0; i < n; i++) {
        size_t indexNextNode = pNodeCurrent->vEdges[i]->pNodeTo->index;
        if (!nodeVisited[indexNextNode]) {
          const float fDistanceFromStartThroughCurrentNode = fDistanceFromStartNode + pNodeCurrent->vEdges[i]->fCost;
          if (fDistanceFromStartThroughCurrentNode < nodeDistanceFromStart[indexNextNode]) nodeDistanceFromStart[indexNextNode] = fDistanceFromStartThroughCurrentNode;
        }
      }

      nodeVisited[pNodeCurrent->index] = true;

      bool bIsVisitedAllNodes = true;
      size_t indexOfLowestCostEdge = 0;
      float fLowestCost = spitfire::math::cINFINITY;
      for (size_t i = 0; i < n; i++) {
        size_t indexNextNode = pNodeCurrent->vEdges[i]->pNodeTo->index;
        if (!nodeVisited[indexNextNode]) {
          bIsVisitedAllNodes = false;
          const float fDistanceFromStartThroughCurrentNode = fDistanceFromStartNode + pNodeCurrent->vEdges[i]->fCost;
          if (fDistanceFromStartThroughCurrentNode < fLowestCost) {
            indexOfLowestCostEdge = i;
            fLowestCost = fDistanceFromStartThroughCurrentNode;
          }
        }
      }

      if (!bIsVisitedAllNodes) {
        fDistanceFromStartNode = fLowestCost;
        pNodeCurrent = pNodeCurrent->vEdges[indexOfLowestCostEdge]->pNodeTo;
        _GetLowestCostPathRecursive(graph, pNodeCurrent, pNodeEnd, fDistanceFromStartNode, path);
      }
    }



    void UnitTest()
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

      const size_t edges[] = {
        0, 1,
        1, 2,
        2, 3,
        2, 4,
        3, 4,
        3, 5,
        4, 6,
        5, 7,
        6, 7,
        4, 8,
        7, 8
      };

      cGraph graph;

      {
        const size_t n = sizeof(nodes) / sizeof(spitfire::math::cVec3);
        for (size_t i = 0; i < n; i++) {
          graph.AddNode(nodes[i], 1.0f);
        }
      }
      {
        const size_t n = sizeof(edges) / sizeof(size_t);
        for (size_t i = 0; i < n; i += 2) {
          graph.AddEdge(edges[i], edges[i + 1], 1.0f);
        }
      }

      graph.Build();


      std::vector<size_t> path;

      cDijkstra dijkstra;
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
  }
}

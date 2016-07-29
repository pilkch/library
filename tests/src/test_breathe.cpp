// Standard headers
#include <iostream>
#include <vector>

// Breathe headers
#include <breathe/game/cAIPathFinder.h>

// Unit test headers
#include "main.h"

void Test_Breathe_AIPathFinder()
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
    ASSERT_TRUE(graph.GetClosestNode(spitfire::math::cVec3(1.0f, 3.0f, 0.0f))->index == 0);
    ASSERT_TRUE(graph.GetClosestNode(spitfire::math::cVec3(1.0f, 3.1f, 0.0f))->index == 0);
    ASSERT_TRUE(graph.GetClosestNode(spitfire::math::cVec3(1.1f, 3.0f, 0.0f))->index == 0);

    ASSERT_TRUE(graph.GetClosestNode(spitfire::math::cVec3(1.0f, 1.0f, 0.0f))->index == 1);
    ASSERT_TRUE(graph.GetClosestNode(spitfire::math::cVec3(-10.1f, -30.0f, 0.0f))->index == 1);

    ASSERT_TRUE(graph.GetClosestNode(spitfire::math::cVec3(10.0f, 4.0f, 0.0f))->index == 5);
    ASSERT_TRUE(graph.GetClosestNode(spitfire::math::cVec3(100.0f, 40.0f, 0.0f))->index == 5);
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

void Test_Breathe()
{
  Test_Breathe_AIPathFinder();
}

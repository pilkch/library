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

#include <gtest/gtest.h>

#include <breathe/game/cAIPathFinder.h>

TEST(breathe, cAIPathFinderTest)
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
  ASSERT_EQ(2, path[0]);
  ASSERT_EQ(3, path[1]);
  ASSERT_EQ(4, path[2]);
  ASSERT_EQ(6, path[3]);
  ASSERT_EQ(7, path[4]);
}

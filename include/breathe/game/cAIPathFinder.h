#ifndef CAIPATHFINDER_H
#define CAIPATHFINDER_H

// Spitfire headers

#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>

// TODO: Hmmmm, maybe polygons would be better than nodes and edges.  Similar algorithm, but there are no edges, only nodes and each node has a 3d volume (Height also for calculating when to crouch etc.).
// Multiple sized/turning/crouching units can all use the same path graph, taking care to avoid areas that they would get stuck in.
// http://www.ai-blog.net/archives/000152.html

namespace breathe
{
  namespace ai
  {
    class cEdge;

    class cNode
    {
    public:
      void Sort(); // Sort the edges travelling away from this node in order of cheapest first

      size_t index;
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

      size_t GetNumberOfNodes() const { return nodes.size(); }
      size_t GetNumberOfEdges() const { return edges.size(); }

      const cNode& GetNode(size_t i) const { ASSERT(i < nodes.size()); return nodes[i]; }
      const cEdge& GetEdge(size_t i) const { ASSERT(i < edges.size()); return edges[i]; }

    private:
      void Sort(); // Sort the edges in cNode::vEdges

      std::vector<cNode> nodes;
      std::vector<cEdge> edges;
    };


    // Dijkstra
    // http://en.wikipedia.org/wiki/Dijkstra%27s_algorithm
    //
    // I wasn't happy with the APIs for any of the current implementations of Dijkstra's algorithm (Even Boost's) so I have written my own implementation based on this blog post:
    // http://ersinacar.com/dijkstra-algorithm_51.html
    //
    // I basically just followed the very helpful notes:
    // 1. Assign to every node a distance value. Set it to zero for our initial node and to infinity for all other nodes.
    // 2. Mark all nodes as unvisited. Set initial node as current.
    // 3. For current node, consider all its unvisited neighbours and calculate their distance (from the initial node). For example, if current node (A) has distance of 6, and an edge connecting it with another node (B) is 2, the distance to B through A will be 6+2=8. If this distance is less than the previously recorded distance (infinity in the beginning, zero for the initial node), overwrite the distance.
    // 4. When we are done considering all neighbours of the current node, mark it as visited. A visited node will not be checked ever again; its distance recorded now is final and minimal.
    // 5. Set the unvisited node with the smallest distance (from the initial node) as the next “current node” and continue from step 3
    //
    // Notes:
    // Each edge is in a single direction, between node0 and node1 there could be hundreds of edges going either direction.
    // Each node only knows about the edges heading away from it.
    class cDijkstra
    {
    public:
      void GetLowestCostPath(const cGraph& graph, size_t nodeStart, size_t nodeEnd, std::vector<size_t>& path);

    private:
      void _GetLowestCostPathRecursive(const cGraph& graph, const cNode* pNodeCurrent, const cNode* pNodeEnd, float fDistanceFromStartNode, std::vector<size_t>& path);

      std::vector<float> nodeDistanceFromStart;
      std::vector<bool> nodeVisited; // NOTE: I'm not sure how standard this is?
    };
  }
}

#endif // CAIPATHFINDER_H

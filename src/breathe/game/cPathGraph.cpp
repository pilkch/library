#include <cmath>

#include <map>
#include <queue>
#include <list>
#include <set>

// Boost includes
#include <boost/shared_ptr.hpp>

#include <breathe/breathe.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>

#include <breathe/game/cPathGraph.h>

namespace breathe
{
  namespace game
  {
    namespace path
    {
      cNode::cNode()
      {
        fWeight = 1.0f;
      }

      cNode::cNode(math::cVec3& v)
        : math::cVec3(v)
      {
        fWeight = 1.0f;
      }

      cNode::cNode(float x, float y, float z)
        : math::cVec3(x, y, z)
      {
        fWeight = 1.0f;
      }

      cNode::~cNode()
      {
      }

      void cNode::Update()
      {
        std::vector<cEdge*>::iterator iter = vEdge.begin();
        std::vector<cEdge*>::iterator iterEnd = vEdge.end();
        cEdge* pEdge = NULL;
        while(iter != iterEnd)
        {
          pEdge = (*iter);
          pEdge->fDistance = (*pEdge->pNode0 - *pEdge->pNode1).GetLength();
        }
      }



      cEdge::cEdge()
      {
        fWeight = 1.0f;
        fDistance = 1.0f;
        pNode0 = pNode1 = NULL;
      }

      cEdge::cEdge(float weight)
      {
        fWeight = weight;
        fDistance = 1.0f;
        pNode0 = pNode1 = NULL;
      }

      cEdge::~cEdge()
      {
      }

      void cEdge::Update()
      {
        if (pNode0 && pNode1) fDistance = (*pNode0 - *pNode1).GetLength();
      }




      void cPathFinder::GetShortestPath(const cPathFinderPersonality& personality, const math::cVec3& start, const math::cVec3& finish, cPath& path) const
      {
        if (IsCloserToFinishThanClosestNode(personality, start, finish)) {
        // We are finished
          path.AddEndPoint(finish);
          return;
        }

      // We don't have any other options, just try to go straight there
        LOG<<"cPathFinder::GetShortestPath FAILED Trying to find the shortest path, just going straight to the finish"<<std::endl;
        path.AddEndPoint(finish);
      }

      void cPathFinder::GetQuickestPath(const cPathFinderPersonality& personality, const math::cVec3& start, const math::cVec3& finish, cPath& path) const
      {
        if (IsCloserToFinishThanClosestNode(personality, start, finish)) {
        // We are finished
          path.AddEndPoint(finish);
          return;
        }

      // We don't have any other options, just try to go straight there
        LOG<<"cPathFinder::GetQuickestPath FAILED Trying to find the shortest path, just going straight to the finish"<<std::endl;
        path.AddEndPoint(finish);
      }




      cPathGraph::cPathGraph()
      {
      }

      cPathGraph::~cPathGraph()
      {
      }



      cNode* cPathGraph::GetNode(const math::cVec3& v3Point)
      {
        std::vector<cNode*>::iterator iter = vNode.begin();
        const std::vector<cNode*>::iterator iterEnd = vNode.end();
        float fClosest = math::cINFINITY;
        cNode* pNode = nullptr;
        float fDistance = 0;
        while (iter != iterEnd) {
          fDistance = (*(*iter) - v3Point).GetLength();
          if (fDistance < fClosest) {
            fClosest = fDistance;
            pNode = (*iter);
          }

          iter++;
        }

        return pNode;
      }

      /*cNode* cPathGraph::GetNodeById(const int id)
      {
        std::vector<cNode*>::iterator iter = vNode.begin();
        const std::vector<cNode*>::iterator iterEnd = vNode.end();
        while(iter != iterEnd) {
          if ((*iter)->uiID == id) return *iter;

          iter=+;
        }

        return nullptr;
      }*/

      void cPathGraph::AddNode(cNode* node)
      {
        vNode.push_back(node);
      }


      void cPathGraph::AddEdgeOneWay(cEdge* edge, cNode* pNodeSource, cNode* pNodeDestination)
      {
        vEdge.push_back(edge);

        // Set each node in our edge
        edge->pNode0 = pNodeSource;
        edge->pNode1 = pNodeDestination;

        // Add this edge to the first node
        edge->pNode0->vEdge.push_back(edge);
      }

      void cPathGraph::AddEdgeTwoWay(cEdge* edge, cNode* pNodeSource, cNode* pNodeDestination)
      {
        vEdge.push_back(edge);

        // Set each node in our edge
        edge->pNode0 = pNodeSource;
        edge->pNode1 = pNodeDestination;

        // Add this edge to the both nodes
        edge->pNode0->vEdge.push_back(edge);
        edge->pNode1->vEdge.push_back(edge);
      }

      /*void cPathGraph::AddEdgeOneWay(cEdge* edge, const int idSource, const int idDestination)
      {
        AddEdgeOneWay(edge, GetNode(idSource), GetNode(idDestination));
      }

      void cPathGraph::AddEdgeTwoWay(cEdge* edge, const int idSource, const int idDestination)
      {
        AddEdgeTwoWay(edge, GetNode(idSource), GetNode(idDestination));
      }*/

      // 1) if the current node if the end node:
      // vPathOut = pNodeBegin;
      // return true;

      // 2) if there is a valid path:
      // vPathOut = at least one node;
      // return true;

      // 3) if there is not a valid path:
      // vPathOut = empty;
      // return false;
      bool cPathGraph::_GetPath(const cNode* pNodeBegin, const cNode* pNodeEnd, std::map<cNode*, bool>& mVisited, node_list& lPathOut)
      {
        cNode* pNodeCurrent = const_cast<cNode*>(pNodeBegin);

        lPathOut.push_back(pNodeCurrent);

        // 1) The current node is the one we are looking for
        if (pNodeCurrent == pNodeEnd) return true;


        // So far we have one visited node, pNodeCurrent
        mVisited[pNodeCurrent] = true;

        std::vector<cEdge*>::iterator iter = vEdge.begin();
        const std::vector<cEdge*>::iterator iterEnd = vEdge.end();
        while (iter != iterEnd) {
          cNode* pNode0 = (*iter)->pNode0;
          cNode* pNode1 = (*iter)->pNode1;

          // pick whichever node is not null, doesn't equal null and hasn't been visited yet
          cNode* pNodeTest = (pNode0 && (pNode0 != pNodeCurrent) && !mVisited[pNode0]) ? pNode0 :
            (pNode1 && (pNode1 != pNodeCurrent) && !mVisited[pNode1]) ? pNode1 : NULL;

          if (pNodeTest != nullptr) {
            // Recursively search this node
            bool bFoundPath = _GetPath(pNodeTest, pNodeEnd, mVisited, lPathOut);
            if (bFoundPath) {
              // 2) There is a valid path to pNodeEnd
              return true;
            }
          }

          iter++;
        }

        // 3) No valid path to pNodeEnd
        return false;
      }

      // 1) if the current node if the end node:
      // vPathOut = pNodeBegin;
      // return true;

      // 2) if there is a valid path:
      // vPathOut = at least one node;
      // return true;

      // 3) if there is not a valid path:
      // vPathOut = empty;
      // return false;
      bool cPathGraph::GetPath(const cNode* pNodeBegin, const cNode* pNodeEnd, node_list& lPathOut)
      {
        lPathOut.clear();

        // Ok, we have to search, set up a vector the size of vNode that will say whether we have visited each node
        std::map<cNode*, bool> mVisited;

        std::vector<cNode*>::iterator iter = vNode.begin();
        std::vector<cNode*>::iterator iterEnd = vNode.end();
        while(iter != iterEnd) {
          mVisited[*iter] = false;

          iter++;
        }

        return _GetPath(pNodeBegin, pNodeEnd, mVisited, lPathOut);
      }

      // This function gets a path from v3Begin to v3End if there is one.
      // 1) if empty()
      // vPathOut = empty;
      // return false;

      // 2) if distrance between v3Begin and v3End is less than the closest node:
      // vPathOut = empty;
      // return true;

      // 3) if the current node if the end node:
      // vPathOut = pNodeBegin;
      // return true;

      // 4) if there is a valid path:
      // vPathOut = at least one node;
      // return true;

      // 5) if there is not a valid path:
      // vPathOut = empty;
      // return false;
      bool cPathGraph::GetPath(const math::cVec3& v3Begin, const math::cVec3& v3End, node_list& lPathOut)
      {
        lPathOut.clear();

        // 1)
        if (empty()) return false;

        // 2)
        cNode* pNodeBegin = GetNode(v3Begin);
        if ((*pNodeBegin - v3Begin).GetLength() > (v3Begin - v3End).GetLength()) return true;

        cNode* pNodeEnd = GetNode(v3End);

        return GetPath(pNodeBegin, pNodeEnd, lPathOut);
      }
    }
  }
}



#ifdef BUILD_DEBUG
#include <breathe/util/unittest.h>

class cPathGraphUnitTest : protected breathe::util::cUnitTestBase
{
public:
  cPathGraphUnitTest() :
    cUnitTestBase("cPathGraphUnitTest")
  {
  }

  void Test()
  {
    breathe::game::path::cPathGraph graph;

    breathe::game::path::cNode* n0 = new breathe::game::path::cNode(1.0f, 1.0f, 0.0f);
    breathe::game::path::cNode* n1 = new breathe::game::path::cNode(2.0f, 8.0f, 0.0f);
    breathe::game::path::cNode* n2 = new breathe::game::path::cNode(3.0f, 1.0f, 0.0f);

    breathe::game::path::cEdge* e0 = new breathe::game::path::cEdge(1.0f);
    breathe::game::path::cEdge* e1 = new breathe::game::path::cEdge(2.0f);

    graph.AddNode(n0);
    graph.AddNode(n1);
    graph.AddNode(n2);

    graph.AddEdgeTwoWay(e0, n0, n1);
    graph.AddEdgeTwoWay(e1, n1, n2);

    breathe::game::path::node_list path;

    graph.GetPath(breathe::math::cVec3(0.0f, 0.0f, 0.0f), breathe::math::cVec3(10.0f, 1.0f, 0.0f), path);

    breathe::game::path::node_list_iterator iter = path.begin();
    breathe::game::path::node_list_iterator iterEnd = path.end();

    breathe::game::path::cNode* pNode = nullptr;
    while(iter != iterEnd) {
      pNode = *iter;

      iter++;
    }
  }
};

cPathGraphUnitTest gPathGraphUnitTest;
#endif // BUILD_DEBUG


#include <cmath>

#include <map>
#include <queue>
#include <list>
#include <set>

#include <BREATHE/cBreathe.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cOctree.h>
#include <BREATHE/MATH/cColour.h>

#include <BREATHE/GAME/cPathGraph.h>

namespace BREATHE
{
	namespace GAME
	{
		namespace PATH
		{
			cPathNode::cPathNode()
			{
				fWeight = 1.0f;
			}
			
			cPathNode::cPathNode(MATH::cVec3& v)
				: MATH::cVec3(v)
			{
				fWeight = 1.0f;
			}

			cPathNode::cPathNode(float x, float y, float z)
				: MATH::cVec3(x, y, z)
			{
				fWeight = 1.0f;
			}

			cPathNode::~cPathNode()
			{
			}

			void cPathNode::Update()
			{
				std::vector<cPathEdge*>::iterator iter = vEdge.begin();
				std::vector<cPathEdge*>::iterator iterEnd = vEdge.end();
				cPathEdge* pEdge = NULL;
				while(iter != iterEnd)
				{
					pEdge = (*iter);
					pEdge->fDistance = (*pEdge->pNode0 - *pEdge->pNode1).GetLength();
				}
			}


			
			cPathEdge::cPathEdge()
			{
				fWeight = 1.0f;
				fDistance = 1.0f;
				pNode0 = pNode1 = NULL;
			}
			
			cPathEdge::cPathEdge(float weight)
			{
				fWeight = weight;
				fDistance = 1.0f;
				pNode0 = pNode1 = NULL;
			}

			cPathEdge::~cPathEdge()
			{
			}
			
			void cPathEdge::Update()
			{
				if(pNode0 && pNode1) fDistance = (*pNode0 - *pNode1).GetLength();
			}


			cPathGraph::cPathGraph()
			{
			}

			cPathGraph::~cPathGraph()
			{
			}
			

			
			cPathNode* cPathGraph::GetNode(const MATH::cVec3& v3Point)
			{
				std::vector<cPathNode*>::iterator iter = vNode.begin();
				std::vector<cPathNode*>::iterator iterEnd = vNode.end();
				float fClosest = MATH::cINFINITY;
				cPathNode* pNode = NULL;
				float fDistance = 0;
				while(iter != iterEnd)
				{
					fDistance = (*(*iter) - v3Point).GetLength();
					if(fDistance < fClosest)
					{
						fClosest = fDistance;
						pNode = (*iter);
					}

					iter++;
				}

				return pNode;
			}

			/*cPathNode* cPathGraph::GetNode(const int id)
			{
				std::vector<cPathNode*>::iterator iter = vNode.begin();
				std::vector<cPathNode*>::iterator iterEnd = vNode.end();
				while(iter != iterEnd)
					if((*iter)->uiID == id)
						return *iter;

				return NULL;
			}*/
			
			void cPathGraph::AddNode(cPathNode* node)
			{
				vNode.push_back(node);
			}

			
			void cPathGraph::AddEdgeOneWay(cPathEdge* edge, cPathNode* pNodeSource, cPathNode* pNodeDestination)
			{
				vEdge.push_back(edge);

				// Set each node in our edge
				edge->pNode0 = pNodeSource;
				edge->pNode1 = pNodeDestination;

				// Add this edge to the first node
				edge->pNode0->vEdge.push_back(edge);
			}

			void cPathGraph::AddEdgeTwoWay(cPathEdge* edge, cPathNode* pNodeSource, cPathNode* pNodeDestination)
			{
				vEdge.push_back(edge);

				// Set each node in our edge
				edge->pNode0 = pNodeSource;
				edge->pNode1 = pNodeDestination;

				// Add this edge to the both nodes
				edge->pNode0->vEdge.push_back(edge);
				edge->pNode1->vEdge.push_back(edge);
			}

			/*void cPathGraph::AddEdgeOneWay(cPathEdge* edge, const int idSource, const int idDestination)
			{
				AddEdgeOneWay(edge, GetNode(idSource), GetNode(idDestination));
			}

			void cPathGraph::AddEdgeTwoWay(cPathEdge* edge, const int idSource, const int idDestination)
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
			bool cPathGraph::_GetPath(const cPathNode* pNodeBegin, const cPathNode* pNodeEnd, 
				std::map<cPathNode*, bool>& mVisited, node_list& lPathOut)
			{
				cPathNode* pNodeCurrent = const_cast<cPathNode*>(pNodeBegin);

				lPathOut.push_back(pNodeCurrent);

				// 1) The current node is the one we are looking for
				if(pNodeCurrent == pNodeEnd) return true;


				// So far we have one visited node, pNodeCurrent
				mVisited[pNodeCurrent] = true;

				std::vector<cPathEdge*>::iterator iter = vEdge.begin();
				std::vector<cPathEdge*>::iterator iterEnd = vEdge.end();
				while(iter != iterEnd)
				{
					cPathNode* pNode0 = (*iter)->pNode0;
					cPathNode* pNode1 = (*iter)->pNode1;

					// pick whichever node is not null, doesn't equal null and hasn't been visited yet
					cPathNode* pNodeTest = (pNode0 && (pNode0 != pNodeCurrent) && !mVisited[pNode0]) ? pNode0 : 
						(pNode1 && (pNode1 != pNodeCurrent) && !mVisited[pNode1]) ? pNode1 : NULL;

					if(pNodeTest)
					{
						// Recursively search this node
						bool bFoundPath = _GetPath(pNodeTest, pNodeEnd, mVisited, lPathOut);
						if(bFoundPath)
						{
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
			bool cPathGraph::GetPath(const cPathNode* pNodeBegin, const cPathNode* pNodeEnd, node_list& lPathOut)
			{
				lPathOut.clear();

				// Ok, we have to search, set up a vector the size of vNode that will say whether we have visited each node
				std::map<cPathNode*, bool> mVisited;
				
				std::vector<cPathNode*>::iterator iter = vNode.begin();
				std::vector<cPathNode*>::iterator iterEnd = vNode.end();
				while(iter != iterEnd)
				{
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
			bool cPathGraph::GetPath(const MATH::cVec3& v3Begin, const MATH::cVec3& v3End, node_list& lPathOut)
			{
				lPathOut.clear();

				// 1)
				if(empty()) return false;

				// 2)
				cPathNode* pNodeBegin = GetNode(v3Begin);
				if((*pNodeBegin - v3Begin).GetLength() > (v3Begin - v3End).GetLength()) return true;

				cPathNode* pNodeEnd = GetNode(v3End);

				return GetPath(pNodeBegin, pNodeEnd, lPathOut);
			}
		}
	}
}

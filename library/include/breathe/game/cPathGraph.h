#ifndef CPATHGRAPH_H
#define CPATHGRAPH_H

// Path Graph for Dijkstra algorithm

namespace breathe
{
	namespace game
	{
		namespace path
		{
			class cPathEdge;
			class cPathNode;

			typedef std::list<cPathNode*> node_list;
			typedef std::list<cPathEdge*>::iterator edge_list_iterator;

			typedef std::vector<cPathEdge*>::iterator edge_vector_iterator;
			typedef std::list<cPathNode*>::iterator node_list_iterator;

			class cPathNode : public math::cVec3
			{
			public:
				cPathNode();
				cPathNode(math::cVec3& v);
				cPathNode(float x, float y, float z);
				virtual ~cPathNode();

				void Update();

				float fWeight;

				std::vector<cPathEdge*> vEdge;
			};
			
			class cPathEdge
			{
			public:
				cPathEdge();
				cPathEdge(float fWeight);
				virtual ~cPathEdge();

				void Update();

				float fWeight;
				float fDistance;

				cPathNode* pNode0;
				cPathNode* pNode1;
			};

			class cPathGraph
			{
			public:
				cPathGraph();
				~cPathGraph();

				bool empty() const { return vNode.empty(); }

				bool GetPath(const cPathNode* pNodeBegin, const cPathNode* pNodeEnd, node_list& lPathOut);
				bool GetPath(const math::cVec3& v3Begin, const math::cVec3& v3End, node_list& lPathOut);

				cPathNode* GetNode(const math::cVec3& v3Point);
				//cPathNode* GetNode(const int id);
				
				void AddNode(cPathNode* node);
				void AddEdgeOneWay(cPathEdge* edge, cPathNode* pNodeSource, cPathNode* pNodeDestination);
				void AddEdgeTwoWay(cPathEdge* edge, cPathNode* pNodeSource, cPathNode* pNodeDestination);
				//void AddEdgeOneWay(cPathEdge* edge, const int idSource, const int idDestination);
				//void AddEdgeTwoWay(cPathEdge* edge, const int idSource, const int idDestination);

				// Removal is a bit of tricky subject
				// a) two way edges only, deletion is easy, just delete the node/edge and all references to it
				// b) one way edges as well as two way edges, have to scan the whole tree to find references to delete
				//void RemoveNode(const int id);
				//void RemoveEdge(const int id);
				//void RemoveNode(const cPathNode* node);
				//void RemoveEdge(const cPathEdge* edge);
				
			private:
				bool _GetPath(const cPathNode* pNodeBegin, const cPathNode* pNodeEnd, 
					std::map<cPathNode*, bool>& mVisited, node_list& lPathOut);

				std::vector<cPathNode*> vNode;
				std::vector<cPathEdge*> vEdge;
			};
		}
	}
}

#endif //CPATHGRAPH_H

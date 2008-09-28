#ifndef CPATHGRAPH_H
#define CPATHGRAPH_H

// Path Graph for Dijkstra algorithm

namespace breathe
{
  namespace game
  {
    namespace path
    {
      class cEdge;
      class cNode;

      typedef std::list<cNode*> node_list;
      typedef std::list<cEdge*>::iterator edge_list_iterator;

      typedef std::vector<cEdge*>::iterator edge_vector_iterator;
      typedef std::list<cNode*>::iterator node_list_iterator;

      class cNode : public math::cSphere
      {
      public:
        cNode();
        explicit cNode(math::cVec3& v);
        cNode(float x, float y, float z);
        virtual ~cNode();

        void Update();

        float fWeight;

        std::vector<cEdge*> vEdge;
      };


      class cEdge
      {
      public:
        cEdge();
        explicit cEdge(float fWeight);
        virtual ~cEdge();

        void Update();

        float GetWeighting() const { return fWeighting0To1 * fDistance; }

      private:
        cNode* pNode0;
        cNode* pNode1;

        float fDistance;
        float fWeighting0To1;
      };


      // A cZone is for specifying for example a room without any obstacles in it
      // It is assumed that each entrance lies within the bounds or very close to the edge of the cBox of the cZone
      class cZone : public math::cBox
      {
      public:
        float GetWeighting(const cNode* pNode0, const cNode* pNode1) const;

      private:
        std::vector<cNode*> entrance; // Where the cZone connects to the rest of the pathgraph

        float fWeighting;
      };

      inline float cZone::GetWeighting(const cNode* pNode0, const cNode* pNode1) const
      {
        return fWeighting * math::GetDistance(pNode0->GetPosition(), pNode1->GetPosition());
      }


      class cPathFinderPersonality
      {
      public:
        cPathFinderPersonality() { SetLandBoundAnimal(); }

        float GetWeightingLand() const { return fWeightingLand; }
        float GetWeightingRoad() const { return fWeightingRoad; }
        float GetWeightingWater() const { return fWeightingWater; }
        float GetWeightingSky() const { return fWeightingSky; }

        void SetHuman();
        void SetLandBoundAnimal();
        void SetWheeledVehicle();

      private:
        float fWeightingLand;
        float fWeightingRoad;
        float fWeightingWater;
        float fWeightingSky;
      };

      inline void cPathFinderPersonality::SetHuman()
      {
        fWeightingLand = 1.0f;
        fWeightingRoad = 1.0f;
        fWeightingWater = 0.3f;
        fWeightingSky = 0.0f;
      }

      inline void cPathFinderPersonality::SetLandBoundAnimal()
      {
        fWeightingLand = 1.0f;
        fWeightingRoad = 0.7f;
        fWeightingWater = 0.2f;
        fWeightingSky = 0.0f;
      }

      inline void cPathFinderPersonality::SetWheeledVehicle()
      {
        fWeightingLand = 0.4f;
        fWeightingRoad = 1.0f;
        fWeightingWater = 0.01f;
        fWeightingSky = 0.0f;
      }

      // A path consists of:
      // nodes.front();
      // ..
      // nodes.back();
      // end
      class cPath
      {
      public:
        void AddNode(cNode* pNode);
        void AddEndPoint(const math::cVec3& end);

        std::list<cNode*>::const_iterator GetNodeBegin() const { return nodes.begin(); }
        std::list<cNode*>::const_iterator GetNodeEnd() const { return nodes.end(); }

        const math::cVec3& GetEnd() const { return end; }

      private:
        // A list of all the official pathgraph nodes along our path
        std::list<cNode*> nodes;

        // There is not normally a node right at the exact point that we wish to get to,
        // so this is the place we want to go after leaving the last node in the node list
        math::cVec3 end;
      };

      inline void cPath::AddNode(cNode* pNode)
      {
        ASSERT(pNode != nullptr);

        nodes.push_back(pNode);
        end = pNode->GetPosition();
      }

      inline void cPath::AddEndPoint(const math::cVec3& _end)
      {
        end = _end;
      }




      class cPathGraph
      {
      public:
        cPathGraph();
        ~cPathGraph();

        bool empty() const { return vNode.empty(); }

        bool GetPath(const cNode* pNodeBegin, const cNode* pNodeEnd, node_list& lPathOut);
        bool GetPath(const math::cVec3& v3Begin, const math::cVec3& v3End, node_list& lPathOut);

        cNode* GetNodeClosestToPoint(const math::cVec3& v3Point) const;
        //cNode* GetNodeById(const int id) const;

        void AddNode(cNode* node);
        void AddEdgeOneWay(cEdge* edge, cNode* pNodeSource, cNode* pNodeDestination);
        void AddEdgeTwoWay(cEdge* edge, cNode* pNodeSource, cNode* pNodeDestination);
        //void AddEdgeOneWay(cEdge* edge, const int idSource, const int idDestination);
        //void AddEdgeTwoWay(cEdge* edge, const int idSource, const int idDestination);

        // Removal is a bit of tricky subject
        // a) two way edges only, deletion is easy, just delete the node/edge and all references to it
        // b) one way edges as well as two way edges, have to scan the whole tree to find references to delete
        //void RemoveNode(const int id);
        //void RemoveEdge(const int id);
        //void RemoveNode(const cNode* node);
        //void RemoveEdge(const cEdge* edge);

      private:
        bool _GetPath(const cNode* pNodeBegin, const cNode* pNodeEnd, std::map<cNode*, bool>& mVisited, node_list& lPathOut);

        std::vector<cNode*> vNode;
        std::vector<cEdge*> vEdge;
      };


      class cPathFinder
      {
      public:
        explicit cPathFinder(const cPathGraph& pathgraph);

        void GetShortestPath(const cPathFinderPersonality& personality, const math::cVec3& start, const math::cVec3& finish, cPath& path) const; // Shortest as the crow flies
        void GetQuickestPath(const cPathFinderPersonality& personality, const math::cVec3& start, const math::cVec3& finish, cPath& path) const; // Quickest taking into account weighting

      private:
        bool IsCloserToFinishThanClosestNode(const cPathFinderPersonality& personality, const math::cVec3& start, const math::cVec3& finish);

        const cPathGraph& pathgraph;
      };

      inline cPathFinder::cPathFinder(const cPathGraph& _pathgraph) :
        pathgraph(_pathgraph)
      {
      }

      inline bool cPathFinder::IsCloserToFinishThanClosestNode(const cPathFinderPersonality& personality, const math::cVec3& start, const math::cVec3& finish)
      {
        cNode* pNode = pathgraph.GetNodeClosestToPoint(start);
        ASSERT(pNode != nullptr);

        const float fStartToClosestNode = math::GetDistance(start, pNode->GetPosition());
        const float fStartToFinish = math::GetDistance(start, finish);
        return (fStartToFinish < fStartToClosestNode);
      }
    }
  }
}

#endif // CPATHGRAPH_H

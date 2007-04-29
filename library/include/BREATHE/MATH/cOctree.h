#ifndef COCTREE_H
#define COCTREE_H

namespace BREATHE
{
	namespace MATH
	{		
		// So we can tell which octant we are talking about
		enum eOctreeNodes
		{
			TOP_LEFT_FRONT,
			TOP_LEFT_BACK,
			TOP_RIGHT_BACK,
			TOP_RIGHT_FRONT,
			BOTTOM_LEFT_FRONT,
			BOTTOM_LEFT_BACK,
			BOTTOM_RIGHT_BACK,
			BOTTOM_RIGHT_FRONT
		};

		// This is our octree class
		class cOctree
		{
		public:
			cOctree();
			~cOctree();

			// The current amount of subdivisions we are currently at.
			// This is used to make sure we don't go over the max amount
			int g_CurrentSubdivision;

			int g_MaxSubdivisions;
			int g_MaxTriangles;
			int g_EndNodeCount;
			int g_TotalNodesDrawn;

			// This returns the center of this node
			cVec3 GetCenter() {	 return m_vCenter;	}

			// This returns the triangle count stored in this node
			int GetTriangleCount()  {   return m_TriangleCount;	}

			// This returns the widht of this node (since it's a cube the height and depth are the same)
			float GetWidth() {	 return m_Width;	}

				// This returns if this node is subdivided or not
			bool IsSubDivided()  {   return m_bSubDivided;	}

			// This sets the initial width, height and depth for the whole scene
			void GetSceneDimensions(cVec3 *pVertices, int numberOfVerts);

			// This takes in the previous nodes center, width and which node ID that will be subdivided
			cVec3 GetNewNodeCenter(cVec3 vCenter, float width, int nodeID);

			// This subdivides a node depending on the triangle and node width
			void CreateNode(cVec3 *pVertices, int numberOfVerts, cVec3 vCenter, float width);

			// This cleans up the new subdivided node creation process, so our code isn't HUGE!
			void CreateNewNode(cVec3 *pVertices,	std::vector<bool> pList, int numberOfVerts,
				  				cVec3 vCenter,	float width,        int triangleCount, int nodeID);

			// This goes through each of the nodes and then draws the end nodes vertices.
			// This function should be called by starting with the root node.
			virtual unsigned int Render(cOctree *pNode) { return 0; }

			// This initializes the data members
			void create(cFrustum * newcFrustum);


			
		public:
			cFrustum * frustum;

			// This tells us if we have divided this node into more sub nodes
			bool m_bSubDivided;

			// This is the size of the cube for this current node
			float m_Width;

			// This holds the amount of triangles stored in this node
			int m_TriangleCount;

			// This is the center (X, Y, Z) point in this node
			cVec3 m_vCenter;

			// This stores the triangles that should be drawn with this node
			cVec3 *m_pVertices;

			// These are the eight nodes branching down from this current node
			cOctree *m_pOctreeNodes[8];	
		};
	}
}

#endif //COCTREE_H

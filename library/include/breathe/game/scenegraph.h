#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

namespace breathe
{
	class cCamera;
	class cObject;

	namespace render
	{
		class cRender;
		class cTexture;
	}

	
	namespace physics
	{
		class cPhysicsObject;
	}

	namespace vehicle
	{
		class cVehicle;
	}

	namespace scenegraph
	{
		class cSceneGraph;

		class cSceneGraphModel : public cObject
		{
		public:
			render::model::cStatic* pModel;

			void Update(sampletime_t currentTime)
			{
				
			}
		};

		class cSceneGraphCubemap
		{
		public:
			math::cVec3 v3Position;

			std::string sFilename;
		};

		class cSceneGraphSpawn
		{
		public:
			cSceneGraphSpawn();

			math::cVec3 v3Position;
			math::cVec3 v3Rotation;
		};


		const unsigned int NODE_INACTIVE=0;
		const unsigned int NODE_UNLOAD=1;
		const unsigned int NODE_ACTIVE=20; //Seconds since the player has been in the vicinity

		class cSceneGraphNode : public cRenderable
		{
			cSceneGraph* pLevel;

		public:
			unsigned int uiStatus;
			
			float fFogDistance;

			math::cColour colourFog;

			std::string sFilename;
			std::string sName;
			std::string sCRC;

			std::vector<cSceneGraphModel*>vModel;
				
			cSceneGraphNode(cSceneGraph* p, std::string sNewFilename);
			~cSceneGraphNode();

			void Load();
			void Unload();

			void Update(sampletime_t currentTime);
			unsigned int Render();
		};
		
		class cSceneGraph
		{
		public:
			cSceneGraph();

		private:
			std::list<cSceneGraphNode*> listOpaque;
			std::map<float, cSceneGraphNode*> mTransparent;

			cSceneGraphNode* pRoot;

			/*
			class cSceneGraphNode
			{
			public:
				bool IsDirty() const { return bIsDirty; }

				void SetDirty() { bIsDirty = true; if(pParent) pParent->SetDirty(); }
				void ClearDirty() { bIsDirty = false; }

			private:
				bool bIsDirty;
			};
			
			class cSceneGraphNode
			{
			public:
				bool IsDirty() const { return uiDirty; }

				void SetDirty() { uiDirty++; if(pParent) pParent->SetDirty(); }
				void ClearDirty() { uiDirty--; if(pParent) pParent->ClearDirty(); }

			private:
				bool uiDirty;
			};
			*/

			void Update()
			{
				listOpaque.clear();
				mTransparent.clear();

				/*for each item in list
					if(opaque)
						listOpaque.add(item);
					else
						mTransparent.add(fDistance, item);*/
			}

			void Render()
			{
				unsigned int uiTriangles = 0;

				// Opaque first
				{
					std::list<cSceneGraphNode*>::iterator iter = listOpaque.begin();
					std::list<cSceneGraphNode*>::iterator iterEnd = listOpaque.end();
					while(iter != iterEnd)
						uiTriangles += (*(iter++))->Render();
				}

				// Transparent second
				{
					std::map<float, cSceneGraphNode*>::iterator iter = mTransparent.begin();
					std::map<float, cSceneGraphNode*>::iterator iterEnd = mTransparent.end();
					while(iter != iterEnd)
						uiTriangles += (*(iter++)).second->Render();
				}
			}
		};
	}
}

#endif //SCENEGRAPH_H

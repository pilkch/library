#ifndef CLEVEL_H
#define CLEVEL_H

namespace BREATHE
{
	class cCamera;
	class cObject;

	namespace RENDER
	{
		class cRender;
		class cTexture;
	}

	
	namespace PHYSICS
	{
		class cPhysicsObject;
	}

	namespace VEHICLE
	{
		class cVehicle;
	}

	class cLevel;



	class cLevelModel : public cObject
	{
	public:
		RENDER::MODEL::cStatic* pModel;

		void Update(float fCurrentTime)
		{
			
		}
	};

	class cLevelCubemap
	{
	public:
		MATH::cVec3 v3Position;

		std::string sFilename;
	};

	class cLevelSpawn
	{
	public:
		cLevelSpawn();

		MATH::cVec3 v3Position;
		MATH::cVec3 v3Rotation;
	};


	const unsigned int NODE_INACTIVE=0;
	const unsigned int NODE_UNLOAD=1;
	const unsigned int NODE_ACTIVE=20; //Seconds since the player has been in the vicinity

	class cLevelNode : public cRenderable
	{
		cLevel *pLevel;

	public:
		unsigned int uiStatus;
		
		float fFogDistance;

		MATH::cColour colourFog;

		std::string sFilename;
		std::string sName;
		std::string sCRC;

		std::vector<cLevelModel * >vModel;
			
		cLevelNode(cLevel *p, std::string sNewFilename);
		~cLevelNode();

		void Load();
		void Unload();

		void Update(float fCurrentTime);
		unsigned int Render();
	};

	class cLevel : public cUpdateable
	{
		float fPreviousTime;

	public:
		// Global level information loaded from xml
		float fWaterLevel;

		std::string sFilename;
		
		// Node Information loaded from xml
		unsigned int uiNodeWidth;
		unsigned int uiNodeHeight;
		unsigned int uiNodeHeightMapPixelWidth;
		unsigned int uiNodeHeightMapPixelHeight;

		float fNodeWidth;


		// Dynamic information
    cLevelNode* pCurrentNode;
		unsigned int uiDisplayNodeName;

		//std::map<std::string, RENDER::MODEL::cHeightmap*> mHeightmap;

		std::list<VEHICLE::cVehicle *> lVehicle;
		std::list<PHYSICS::cPhysicsObject*> lPhysicsObject;

		std::vector<cLevelNode*>vNode; //vModel.size()=uiWidth*uiHeight;

		std::vector<cLevelSpawn*>vSpawn;
		
		std::vector<cLevelCubemap * >vCubemap;

		cLevel();
		~cLevel();

		bool Load(std::string sNewFilename);
		void LoadNode(std::string sNewFilename);
		void LoadCubemap(std::string sFilename);

		void Update(float fCurrentTime);
		unsigned int Render(float fCurrentTime);

		cLevelSpawn GetSpawn(); //Get a random spawn
		cLevelSpawn GetSpawn(MATH::cVec3 &p); //Get closest spawn to requested position

		
		void AddVehicle(VEHICLE::cVehicle *v);
		void RemoveVehicle(VEHICLE::cVehicle *v);

		void AddPhysicsObject(PHYSICS::cPhysicsObject *d);
		void RemovePhysicsObject(PHYSICS::cPhysicsObject *d);

		unsigned int RenderVehicles(float fCurrentTime, VEHICLE::cVehicle *pOwnVehicle);

		RENDER::cTexture *FindClosestCubeMap(MATH::cVec3 pos);

		VEHICLE::cVehicle *FindClosestVehicle(MATH::cVec3 pos, float fMaxDistance);
	};
	
	class cSceneGraph
	{
	public:
		std::list<cLevelNode*> listOpaque;
		std::map<float, cLevelNode*> mTransparent;

		cLevelNode* pRoot;

		/*
		class cLevelNode
		{
		public:
			bool IsDirty() { return bIsDirty; }

			void SetDirty() { bIsDirty = true; if(pParent) pParent->SetDirty(); }
			void ClearDirty() { bIsDirty = false; }

		private:
			bool bIsDirty;
		};
		
		class cLevelNode
		{
		public:
			bool IsDirty() { return uiDirty; }

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
				std::list<cLevelNode*>::iterator iter = listOpaque.begin();
				std::list<cLevelNode*>::iterator iterEnd = listOpaque.end();
				while(iter != iterEnd)
					uiTriangles += (*(iter++))->Render();
			}

			// Transparent second
			{
				std::map<float, cLevelNode*>::iterator iter = mTransparent.begin();
				std::map<float, cLevelNode*>::iterator iterEnd = mTransparent.end();
				while(iter != iterEnd)
					uiTriangles += (*(iter++)).second->Render();
			}
		}
	};
}

extern BREATHE::cLevel* pLevel;

#endif //CLEVEL_H

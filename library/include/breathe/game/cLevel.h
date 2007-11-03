#ifndef CLEVEL_H
#define CLEVEL_H

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

	class cLevel;



	class cLevelModel : public cObject
	{
	public:
		render::model::cStatic* pModel;

		void Update(sampletime_t currentTime)
		{
			
		}
	};

	class cLevelCubemap
	{
	public:
		math::cVec3 v3Position;

		std::string sFilename;
	};

	class cLevelSpawn
	{
	public:
		cLevelSpawn();

		math::cVec3 v3Position;
		math::cVec3 v3Rotation;
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

		math::cColour colourFog;

		std::string sFilename;
		std::string sName;
		std::string sCRC;

		std::vector<cLevelModel * >vModel;
			
		cLevelNode(cLevel *p, std::string sNewFilename);
		~cLevelNode();

		void Load();
		void Unload();

		void Update(sampletime_t currentTime);
		unsigned int Render();
	};

	class cLevel : public cUpdateable
	{
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

		//std::map<std::string, render::model::cHeightmap*> mHeightmap;

		std::list<vehicle::cVehicle *> lVehicle;
		std::list<physics::cPhysicsObject*> lPhysicsObject;

		std::vector<cLevelNode*>vNode; //vModel.size()=uiWidth*uiHeight;

		std::vector<cLevelSpawn*>vSpawn;
		
		std::vector<cLevelCubemap * >vCubemap;

		cLevel();
		~cLevel();

	private:
		bool LoadXML(std::string sNewFilename);

	public:
		bool Load(std::string sNewFilename);
		void LoadNode(std::string sNewFilename);
		void LoadCubemap(std::string sFilename);

		void Update(sampletime_t currentTime);
		unsigned int Render(sampletime_t currentTime);

		cLevelSpawn GetSpawn(); //Get a random spawn
		cLevelSpawn GetSpawn(math::cVec3 &p); //Get closest spawn to requested position

		
		void AddVehicle(vehicle::cVehicle *v);
		void RemoveVehicle(vehicle::cVehicle *v);

		void AddPhysicsObject(physics::cPhysicsObject *d);
		void RemovePhysicsObject(physics::cPhysicsObject *d);

		unsigned int RenderVehicles(sampletime_t currentTime, vehicle::cVehicle *pOwnVehicle);

		render::cTexture *FindClosestCubeMap(math::cVec3 pos);

		vehicle::cVehicle *FindClosestVehicle(math::cVec3 pos, float fMaxDistance);

	private:
		uint32_t previousTime;
	};
}

extern breathe::cLevel* pLevel;

#endif //CLEVEL_H

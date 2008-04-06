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

		string_t sFilename;
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
	public:
		cLevelNode(cLevel *p, const string_t& sNewFilename);
		~cLevelNode();

		void Load();
		void Unload();

		void Update(sampletime_t currentTime);
		unsigned int Render();


		unsigned int uiStatus;
		
		float fFogDistance;

		math::cColour colourFog;

		string_t sFilename;
		string_t sName;
		std::string sCRC;

		std::vector<cLevelModel * >vModel;

  private:
		cLevel *pLevel;
		render::model::cStatic* pModel;
	};

	class cLevel : public cUpdateable
	{
	public:
    cLevel();
    ~cLevel();

		// Global level information loaded from xml
		float fWaterLevel;

		string_t sFilename;
		
		// Node Information loaded from xml
		unsigned int uiNodeWidth;
		unsigned int uiNodeHeight;
		unsigned int uiNodeHeightMapPixelWidth;
		unsigned int uiNodeHeightMapPixelHeight;

		float fNodeWidth;


		// Dynamic information
    cLevelNode* pCurrentNode;
		unsigned int uiDisplayNodeName;

		//std::map<string_t, render::model::cHeightmap*> mHeightmap;

		std::list<vehicle::cVehicle *> lVehicle;
		std::list<physics::cPhysicsObject*> lPhysicsObject;

		std::vector<cLevelNode*>vNode; //vModel.size()=uiWidth*uiHeight;

		std::vector<cLevelSpawn*>vSpawn;
		
		std::vector<cLevelCubemap * >vCubemap;

	private:
		bool LoadXML(const string_t& sNewFilename);

	public:
		bool Load(const string_t& sNewFilename);
		void LoadNode(const string_t& sNewFilename);
		void LoadCubemap(const string_t& sFilename);

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

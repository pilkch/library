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
		class cPhysics;
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
		MODEL::cStatic* pModel;

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

	class cLevelNode : public cUpdateable
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

		void Update(float fTime);
		unsigned int Render(float fTime);
	};

	class cLevel : public cUpdateable
	{
		float fPreviousTime;

	public:
		RENDER::cRender *pRender;

		unsigned int uiWidth;
		unsigned int uiHeight;

		float fNodeWidth;
		float fWaterLevel;

		std::string sFilename;

		//std::map<std::string, cModel_Animation> mAnimation;
		std::map<std::string, MODEL::cStatic*> mStatic;
		//std::map<std::string, cModel_Roam> mRoam;

		/*dSpaceID statics_;
    dSpaceID actors_;
    dSpaceID triggers_;
    dJointGroupID collJoints_;
    std::vector< Obj * > staticObjs_;
    std::vector< Obj * > movingObjs_;
    std::vector< Obj * > ephemerealObjs_;
    std::map< std::wstring, RefPointer< TriMeshData > > triMeshes_;*/

		std::list<VEHICLE::cVehicle *> lVehicle;
		std::list<PHYSICS::cPhysicsObject*> lPhysicsObject;

		std::vector<cLevelNode*>vNode; //vModel.size()=uiWidth*uiHeight;

		std::vector<cLevelSpawn*>vSpawn;
		
		std::vector<cLevelCubemap * >vCubemap;

		cLevel(RENDER::cRender *p);
		~cLevel();

		bool Load(std::string sNewFilename);
		void LoadNode(std::string sNewFilename);
		void LoadCubemap(std::string sFilename);

		void TransformModels();

		MODEL::cStatic *AddModel(std::string sNewFilename);
		MODEL::cStatic *GetModel(std::string sNewFilename);

		void Update(float fTime);
		unsigned int Render(float fTime);

		cLevelSpawn GetSpawn(); //Get a random spawn
		cLevelSpawn GetSpawn(MATH::cVec3 &p); //Get closest spawn to requested position

		
		void AddVehicle(VEHICLE::cVehicle *v);
		void RemoveVehicle(VEHICLE::cVehicle *v);

		void AddPhysicsObject(PHYSICS::cPhysicsObject *d);
		void RemovePhysicsObject(PHYSICS::cPhysicsObject *d);

		unsigned int RenderVehicles(float fCurrentTime, VEHICLE::cVehicle *pOwnVehicle);
		unsigned int RenderStaticModel(MODEL::cStatic *p, MATH::cVec3 pos, MATH::cColour colour=MATH::cColour(0.0f, 0.0f, 0.0f, 0.0f));

		RENDER::cTexture *FindClosestCubeMap(MATH::cVec3 pos);

		VEHICLE::cVehicle *FindClosestVehicle(MATH::cVec3 pos, float fMaxDistance);

		/*      Obj * makeObject( Params & p, std::wstring const & name, std::wstring const & type );
      Obj * makeBox( Params & p, dBodyID parent );
      Obj * makeSphere( Params & p, dBodyID parent );
      Obj * makeCapsule( Params & p, dBodyID parent );
      Obj * makePlane( Params & p, dBodyID parent );
      Obj * makeMesh( Params & p, dBodyID parent );

      void embellish( Params & p, Obj * o );
      void addBehavior( Params & p, Obj * o, std::wstring const & name );
      void makeHinge( Params & p, dBodyID parent );
      void makeBall( Params & p, dBodyID parent );
      void makeUniversal( Params & p, dBodyID parent );
      void makeSlider( Params & p, dBodyID parent );

      static float ContactDistanceSq( dContact const & a, dContact const & b );
      static void JoinContact( dContact const & a, dContact & b );
      static void nearCallbackActors( void * param, dGeomID g1, dGeomID g2 );
      static void nearCallbackStatics( void * param, dGeomID g1, dGeomID g2 );
      static void nearCallbackTriggers( void * param, dGeomID g1, dGeomID g2 );*/
	};
}

#endif //CLEVEL_H

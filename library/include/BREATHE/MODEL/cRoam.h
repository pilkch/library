#ifndef CMODELROAM_H
#define CMODELROAM_H

namespace BREATHE
{
	namespace MODEL
	{
		#define MAP_1024

		// ------- 1024x1024 MAP -------
		#define MAP_SIZE (1024)
		#define NUM_PATCHES_PER_SIDE (16)

		// How many TriTreeNodes should be allocated?
		#define POOL_SIZE (25000)

		// Some more definitions
		#define PATCH_SIZE (MAP_SIZE/NUM_PATCHES_PER_SIDE)
		#define ROAM_TEXTURE_SIZE (128)

		// Mesh properties...
		#define g_nNumVertsAlongX 64
		#define g_nNumVertsAlongZ 64
		#define g_nMeshVertCount (g_nNumVertsAlongX-1) * (g_nNumVertsAlongZ-1) * 2

		extern void calcNormal(float v[3][3], float out[3]);
		extern void ReduceToUnit(float vector[3]);

		enum {SKYBOX_BACK,
			SKYBOX_FRONT,
			SKYBOX_BOTTOM,
			SKYBOX_TOP,
			SKYBOX_LEFT,
			SKYBOX_RIGHT};

		class cRoam : public cModel
		{
		protected:
			//void SetTextureCoord(float x, float z);
			static void RenderSkybox();
			static void RenderWater();
			static void RenderTrees();

			static int GetNextTriNode() { return m_NextTriNode; }
			static void SetNextTriNode( int nNextNode ) { m_NextTriNode = nNextNode; }

			virtual void Reset();
			virtual void Tessellate();

		public:
			cRoam()
			{
				// Beginning frame varience (should be high, it will adjust automatically)
				gFrameVariance = 50;

				// Desired number of Binary Triangle tessellations per frame.
				// This is not the desired number of triangles rendered!
				// There are usually twice as many Binary Triangle structures as there are rendered triangles.
				gDesiredTris = 2000;
			}

			~cRoam()
			{
				if(m_HeightMap)
					delete [] m_HeightMap;
			}
			
			int Load(char *directory);
			int Render();

			static cTriTreeNode *AllocateTri();
			static void SetShaderConstants(float x, float y, float z);

			virtual bool Init(unsigned char *hMap, int width, int height, int trees);
				
			float Height(int x, int z);
			float Height(float x, float z);

			void Update(float fTime);
			void UpdateTriangles();
			void UpdateTrees(float time);
			void UpdateWater(float time);

		protected:
			unsigned char * m_HeightMap;										// HeightMap of the Landscape
			cRoamPatch m_Patches[NUM_PATCHES_PER_SIDE][NUM_PATCHES_PER_SIDE];	// Array of patches

			static int m_NextTriNode;										// Index to next free TriTreeNode
			static cTriTreeNode m_TriPool[POOL_SIZE];						// Pool of TriTree nodes for splitting

		public:
			static bool renderWireframe, renderUpdate, renderStrips, renderVertexShader, renderTrees,
				renderDetail, renderCube, renderLightmap, renderFog, renderWater, renderSky,
				updateTrees, updateTerrain, updateWater;

			static unsigned int VertexShader, 
				g_location_currentAngle, g_location_myTexture0, g_location_myTexture1;

			static int nSectors, width, height, renderedPatches, renderedTriangles, gDesiredTris, 
				textureTerrain, textureDetail, textureWater, textureWaterReflection, textureCube, 
				waterReflectionWidth, waterSize, waterWidth, waterHeight, waterWave, 
				textureLightmap, textureSkybox[6];

			static float scale, oneOverScale, gFrameVariance, detailscale, 
				g_fCurrentAngle, g_fSpeedOfRotation,
				g_fElpasedTime, g_fSpinX, g_fSpinY, g_fMeshLengthAlongX, g_fMeshLengthAlongZ;

			static double g_dCurTime, g_dLastTime;

			static BREATHE::MATH::cFrustum *pFrustum;

			std::vector <cTriangle*> triangles;
		};
	}
}

#endif //CMODELROAM_H

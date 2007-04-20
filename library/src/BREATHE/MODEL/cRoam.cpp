#include <stdio.h>
#include <math.h>

#include <vector>
#include <map>




#include <windows.h>
#include <gl\gl.h>

//#include <cCamera.h>





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

#include <BREATHE/UTIL/cBase.h>
#include <BREATHE/MODEL/cMesh.h>
#include <BREATHE/MODEL/cModel.h>
#include <BREATHE/MODEL/cStatic.h>
#include <BREATHE/MODEL/cRoamPatch.h>
#include <BREATHE/MODEL/cRoam.h>

namespace BREATHE
{
	namespace MODEL
	{
		const float billboardDistance=2500.0f;
		const float crossoverDistance=1000.0f;
		const float modelDistance=billboardDistance-crossoverDistance;

		bool					cRoam::renderFog;
		bool					cRoam::renderUpdate;
		bool					cRoam::renderStrips;
		bool					cRoam::renderVertexShader;
		bool					cRoam::renderDetail;
		bool					cRoam::renderCube;
		bool					cRoam::renderLightmap;
		bool					cRoam::renderSky;
		bool					cRoam::renderWater;
		bool					cRoam::renderTrees;
		bool 					cRoam::renderWireframe = false;

		bool					cRoam::updateWater;
		bool					cRoam::updateTerrain;
		bool					cRoam::updateTrees;

		unsigned int	cRoam::VertexShader;

		int						cRoam::nSectors;
		int						cRoam::m_NextTriNode;
		int						cRoam::renderedPatches;
		int						cRoam::renderedTriangles;
		int						cRoam::height;
		int						cRoam::width;
		int						cRoam::gDesiredTris;
		int						cRoam::textureTerrain;
		int						cRoam::textureDetail;
		int						cRoam::textureCube;
		int						cRoam::textureLightmap;
		int						cRoam::textureSkybox[6];

		int						cRoam::textureWater;
		int						cRoam::textureWaterReflection;
		int						cRoam::waterReflectionWidth=256;
		int						cRoam::waterSize=64;
		int						cRoam::waterWidth=10;
		int 					cRoam::waterHeight;
		int 					cRoam::waterWave;

		float 				cRoam::gFrameVariance;
		float 				cRoam::detailscale;
		float 				cRoam::scale=5.0f;
		float 				cRoam::oneOverScale=1.0f/cRoam::scale;

		cTriTreeNode	cRoam::m_TriPool[POOL_SIZE];

		GLuint 				cRoam::g_location_myTexture0;
		GLuint 				cRoam::g_location_myTexture1;
		GLuint 				cRoam::g_location_currentAngle;
		float 				cRoam::g_fCurrentAngle;
		float 				cRoam::g_fSpeedOfRotation = 10.0f;
		float 				cRoam::g_fElpasedTime;
		double 				cRoam::g_dCurTime;
		double 				cRoam::g_dLastTime;
		float 				cRoam::g_fSpinX = 0.0f;
		float 				cRoam::g_fSpinY = 0.0f;
		float 				cRoam::g_fMeshLengthAlongX = 800.0f;
		float 				cRoam::g_fMeshLengthAlongZ = 800.0f;

		MATH::cFrustum *		cRoam::pFrustum;



		float cRoam::Height(int x, int y)
		{
			//int i=0, x=0, y=0, aw, ah, aw=nSectors*width, ah=nSectors*height;

			//if(X<a || Y<y || X > aw || Y > ah) 
			//		return 0.0f;
			
			//for(y=0;y<nSectors;y++)
			//	for(x=0;x<nSectors;x++)
			//		if(X>=level[x+y*nSectors].x && Y>=level[x+y*nSectors].y &&
			//			X<ax && Y<ay)
			//			return level[X + (Y * width)].Height();

			if(x<0 || y<0 || x > width || y > height)
				return 0.0f;

			return cRoam::scale*m_HeightMap[x + (y * width)];
		}

		float cRoam::Height(float x, float z)
		{
			int		xi,zi;
			float   h0, h1, h3;
			float	xfrac, zfrac;

			xi = (int)x;
			zi = (int)z;
			
			if(xi<0 || zi<0 || xi > width || zi > height)
				return 0.0f;

			//   0---1    
			//   |   |    
			//   3---2    

			h0 = m_HeightMap[xi + (zi * width)];
			h1 = m_HeightMap[xi+1 + (zi * width)];
			h3 = m_HeightMap[xi + (zi * width+width)];

			xfrac = x - (float)xi;
			zfrac = z - (float)zi;

			// calculate interpolated ground height
			return cRoam::scale*(h0 + xfrac*(h1-h0) + zfrac*(h3-h0));
		}

		// ---------------------------------------------------------------------
		// Initialize all patches
		//
		bool cRoam::Init(unsigned char *hMap, int width, int height, int trees)
		{
			this->width=width;
			this->height=height;

			cRoamPatch *patch;
			int x=0,y=0,z=0;
			float w=100.0f;
			bool ChangeSides=false;

			// Store the Height Field array
			m_HeightMap = new unsigned char [width*height];

			for ( y=0; y < height; y++)
				for ( x=0; x < width; x++ )
					m_HeightMap[y*width+x]=hMap[y*width+x];

			// Initialize all terrain patches
			for ( y=0; y < NUM_PATCHES_PER_SIDE; y++)
				for ( x=0; x < NUM_PATCHES_PER_SIDE; x++ )
				{
					patch = &(m_Patches[y][x]);
					patch->Init( x*PATCH_SIZE, y*PATCH_SIZE, 
						x*PATCH_SIZE, y*PATCH_SIZE, m_HeightMap);
					patch->ComputeVariance();
				}

			renderTrees=true;
			renderSky=true;
			renderWater=true;
			renderUpdate=true;
			renderFog=true;
			renderDetail=true;
			renderCube=true;
			renderLightmap=true;
			
			updateTerrain=true;
			updateTrees=true;
			updateWater=true;

			waterHeight=100;

			float X=0.0f, Y=0.0f,Z=0.0f;
			float HEIGHT_MAP_SIZE=g_nNumVertsAlongZ*w;
			int I=0;
			int tc=textureCube;

			detailscale=10.0f;
			
			return true;
		}

		// ---------------------------------------------------------------------
		// Allocate a TriTreeNode from the pool.
		//
		cTriTreeNode *cRoam::AllocateTri()
		{
			cTriTreeNode *pTri;

			// IF we've run out of TriTreeNodes, just return NULL (this is handled gracefully)
			if ( m_NextTriNode >= POOL_SIZE )
				return NULL;

			pTri = &(m_TriPool[m_NextTriNode++]);
			pTri->LeftChild = pTri->RightChild = NULL;

			return pTri;
		}

		// ---------------------------------------------------------------------
		// Reset all patches, recompute variance if needed
		//
		void cRoam::Reset()
		{
			//
			// Perform simple visibility culling on entire patches.
			//   - Define a triangle set back from the frustum by one patch size, following
			//     the angle of the frustum.
			//   - A patch is visible if it's center point is included in the angle: Left,Eye,Right
			//   - This visibility test is only accurate if the frustum cannot look up or down significantly.
			//
			int eyeX = (int)(cRoam::pFrustum->eye.x - PATCH_SIZE * sinf( cRoam::pFrustum->look.z * MATH::cPI_DIV_180 ));
			int eyeY = (int)(cRoam::pFrustum->eye.z + PATCH_SIZE * cosf( cRoam::pFrustum->look.z * MATH::cPI_DIV_180 ));

			int leftX  = (int)(eyeX + 100.0f * sinf( (cRoam::pFrustum->look.z-cRoam::pFrustum->fov) * MATH::cPI_DIV_180 ));
			int leftY  = (int)(eyeY - 100.0f * cosf( (cRoam::pFrustum->look.z-cRoam::pFrustum->fov) * MATH::cPI_DIV_180 ));

			int rightX = (int)(eyeX + 100.0f * sinf( (cRoam::pFrustum->look.z+cRoam::pFrustum->fov) * MATH::cPI_DIV_180 ));
			int rightY = (int)(eyeY - 100.0f * cosf( (cRoam::pFrustum->look.z+cRoam::pFrustum->fov) * MATH::cPI_DIV_180 ));

			int X, Y;
			cRoamPatch *patch;

			// Set the next free triangle pointer back to the beginning
			SetNextTriNode(0);

			// Reset rendered triangle count.
			renderedTriangles = 0;

			// Go through the patches performing resets, compute variances, and linking.
			for ( Y=0; Y < NUM_PATCHES_PER_SIDE; Y++ )
				for ( X=0; X < NUM_PATCHES_PER_SIDE; X++)
				{
					patch = &(m_Patches[Y][X]);
					
					// Reset the patch
					patch->Reset();
					patch->SetVisibility( eyeX, eyeY, leftX, leftY, rightX, rightY );
					
					// Check to see if this patch has been deformed since last frame.
					// If so, recompute the varience tree for it.
					if ( patch->isDirty() )
						patch->ComputeVariance();

					if ( patch->isVisibile() )
					{
						// Link all the patches together.
						if ( X > 0 )
							patch->GetBaseLeft()->LeftNeighbor = m_Patches[Y][X-1].GetBaseRight();
						else
							patch->GetBaseLeft()->LeftNeighbor = NULL;		// Link to bordering cRoam here..

						if ( X < (NUM_PATCHES_PER_SIDE-1) )
							patch->GetBaseRight()->LeftNeighbor = m_Patches[Y][X+1].GetBaseLeft();
						else
							patch->GetBaseRight()->LeftNeighbor = NULL;		// Link to bordering cRoam here..

						if ( Y > 0 )
							patch->GetBaseLeft()->RightNeighbor = m_Patches[Y-1][X].GetBaseRight();
						else
							patch->GetBaseLeft()->RightNeighbor = NULL;		// Link to bordering cRoam here..

						if ( Y < (NUM_PATCHES_PER_SIDE-1) )
							patch->GetBaseRight()->RightNeighbor = m_Patches[Y+1][X].GetBaseLeft();
						else
							patch->GetBaseRight()->RightNeighbor = NULL;	// Link to bordering cRoam here..
					}
				}

		}

		// ---------------------------------------------------------------------
		// Create an approximate mesh of the landscape.
		//
		void cRoam::Tessellate()
		{
			// Perform Tessellation
			int nCount;
			cRoamPatch *patch = &(m_Patches[0][0]);
			for (nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; nCount++, patch++ )
			{
				if (patch->isVisibile())
					patch->Tessellate( );
			}
		}

		void cRoam::UpdateTriangles()
		{
			int nCount=0;
			cRoamPatch *patch = &(m_Patches[0][0]);
			triangles.clear();
			uiTriangles=0;
			renderedPatches=0;

			for(nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; nCount++, patch++ )
				if (patch->isVisibile())
				{
					patch->AddTriangles();
					renderedPatches++;
				}
		}

		int cRoam::Render()
		{
			unsigned int nCount;
			renderedPatches=0;
			uiTriangles=0;
			cRoamPatch *patch = &(m_Patches[0][0]);

			if(renderFog)
				glEnable(GL_FOG);
			
			// Activate the first texture ID and bind the tree background to it
		//	glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureTerrain);

			// If we want detail texturing on, let's render the second texture
			if(renderDetail)
			{
				// Activate the second texture ID and bind the fog texture to it
		//		glActiveTextureARB(GL_TEXTURE1_ARB);
				glEnable(GL_TEXTURE_2D);
				
				// Here we turn on the COMBINE properties and increase our RGB
				// gamma for the detail texture.  2 seems to work just right.
		//		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		//		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
				
				// Bind the detail texture
		//		glBindTexture(GL_TEXTURE_2D, textureDetail);
			
				// Now we want to enter the texture matrix.  This will allow us
				// to change the tiling of the detail texture.
				glMatrixMode(GL_TEXTURE);

					// Reset the current matrix and apply our chosen scale value
					glLoadIdentity();
					glScalef(detailscale, detailscale, 1);

				// Leave the texture matrix and set us back in the model view matrix
				glMatrixMode(GL_MODELVIEW);
			}

			// If we want detail texturing on, let's render the second texture
			if(renderLightmap)
			{
				// Activate the second texture ID and bind the fog texture to it
		//		glActiveTextureARB(GL_TEXTURE2_ARB);
				glEnable(GL_TEXTURE_2D);
				
				// Here we turn on the COMBINE properties and increase our RGB
				// gamma for the detail texture.  2 seems to work just right.
		//		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		//		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
				
				// Bind the detail texture
				glBindTexture(GL_TEXTURE_2D, textureLightmap);
			}

			if(renderStrips)
				glBegin( GL_TRIANGLE_STRIP );
			else
				glBegin( GL_TRIANGLES);

					UpdateTriangles();
					if(renderFog)
						for(nCount=0; nCount < uiTriangles; nCount++)
							triangles[nCount]->RenderFog();
					else
						for(nCount=0; nCount < uiTriangles; nCount++)
							triangles[nCount]->Render();
				
					for (nCount=0; nCount < NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE; nCount++, patch++ )
						if (patch->isVisibile())
						{
							patch->Render();
							renderedPatches++;
						}
			glEnd();

			// Turn the third multitexture pass off
		//	glActiveTextureARB(GL_TEXTURE2_ARB);
				glDisable(GL_TEXTURE_2D);

			// Turn the second multitexture pass off
		//	glActiveTextureARB(GL_TEXTURE1_ARB);
				glDisable(GL_TEXTURE_2D);

			// Turn the first multitexture pass off
		//	glActiveTextureARB(GL_TEXTURE0_ARB);		
				glDisable(GL_TEXTURE_2D);

			glDisable(GL_FOG);

			renderedTriangles=uiTriangles;

			// Check to see if we got close to the desired number of triangles.
			// Adjust the frame variance to a better value.
			if ( GetNextTriNode() != gDesiredTris )
				gFrameVariance += ((float)GetNextTriNode() - (float)gDesiredTris) / (float)gDesiredTris;

			// Bounds checking.
			if ( gFrameVariance < 0 )
				gFrameVariance = 0;

			return renderedTriangles;
		}


		void cRoam::Update(float time)
		{	
			if(updateTerrain)
			{
				Reset();
				Tessellate();
			}
		}

		int cRoam::Load(char *directory)
		{
			return 0;
		}
	}
}

#include <cstdio>
#include <cmath>

#include <list>
#include <vector>
#include <map>
#include <fstream>

#include <GL/Glee.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

// Breathe
#include <BREATHE/cBreathe.h>
#include <BREATHE/UTIL/cLog.h>

//#include <cCamera.h>

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
#include <BREATHE/RENDER/cVertexBufferObject.h>
#include <BREATHE/RENDER/MODEL/cMesh.h>
#include <BREATHE/RENDER/MODEL/cModel.h>
#include <BREATHE/RENDER/MODEL/cStatic.h>
#include <BREATHE/RENDER/MODEL/cHeightmapPatch.h>
#include <BREATHE/RENDER/MODEL/cHeightmap.h>


namespace BREATHE
{
	namespace RENDER
	{
		namespace MODEL
		{/*
			// Split a single Triangle and link it into the mesh.
			// Will correctly force-split diamonds.
			void cHeightmapPatch::Split(cTriTreeNode *tri)
			{
				// We are already split, no need to do it again.
				if (tri->LeftChild)
					return;

				// If this triangle is not in a proper diamond, force split our base neighbor
				if ( tri->BaseNeighbor && (tri->BaseNeighbor->BaseNeighbor != tri) )
					Split(tri->BaseNeighbor);

				// Create children and link into mesh
				tri->LeftChild  = cHeightmap::AllocateTri();
				tri->RightChild = cHeightmap::AllocateTri();

				// If creation failed, just exit.
				if ( !tri->LeftChild )
					return;

				// Fill in the information we can get from the parent (neighbor pointers)
				tri->LeftChild->BaseNeighbor  = tri->LeftNeighbor;
				tri->LeftChild->LeftNeighbor  = tri->RightChild;

				tri->RightChild->BaseNeighbor  = tri->RightNeighbor;
				tri->RightChild->RightNeighbor = tri->LeftChild;

				// Link our Left Neighbor to the new children
				if (tri->LeftNeighbor != NULL)
				{
					if (tri->LeftNeighbor->BaseNeighbor == tri)
						tri->LeftNeighbor->BaseNeighbor = tri->LeftChild;
					else if (tri->LeftNeighbor->LeftNeighbor == tri)
						tri->LeftNeighbor->LeftNeighbor = tri->LeftChild;
					else if (tri->LeftNeighbor->RightNeighbor == tri)
						tri->LeftNeighbor->RightNeighbor = tri->LeftChild;
					else
						;// Illegal Left Neighbor!
				}

				// Link our Right Neighbor to the new children
				if (tri->RightNeighbor != NULL)
				{
					if (tri->RightNeighbor->BaseNeighbor == tri)
						tri->RightNeighbor->BaseNeighbor = tri->RightChild;
					else if (tri->RightNeighbor->RightNeighbor == tri)
						tri->RightNeighbor->RightNeighbor = tri->RightChild;
					else if (tri->RightNeighbor->LeftNeighbor == tri)
						tri->RightNeighbor->LeftNeighbor = tri->RightChild;
					else
						;// Illegal Right Neighbor!
				}

				// Link our Base Neighbor to the new children
				if (tri->BaseNeighbor != NULL)
				{
					if ( tri->BaseNeighbor->LeftChild )
					{
						tri->BaseNeighbor->LeftChild->RightNeighbor = tri->RightChild;
						tri->BaseNeighbor->RightChild->LeftNeighbor = tri->LeftChild;
						tri->LeftChild->RightNeighbor = tri->BaseNeighbor->RightChild;
						tri->RightChild->LeftNeighbor = tri->BaseNeighbor->LeftChild;
					}
					else
						Split( tri->BaseNeighbor);  // Base Neighbor (in a diamond with us) was not split yet, so do that now.
				}
				else
				{
					// An edge triangle, trivial case.
					tri->LeftChild->RightNeighbor = NULL;
					tri->RightChild->LeftNeighbor = NULL;
				}
			}

			// ---------------------------------------------------------------------
			// Tessellate a Patch.
			// Will continue to split until the variance metric is met.
			//
			void cHeightmapPatch::RecursTessellate( cTriTreeNode *tri,
										int leftX,  int leftY,
										int rightX, int rightY,
										int apexX,  int apexY,
										int node )
			{
				float TriVariance;
				int centerX = (leftX + rightX)>>1; // Compute X coordinate of center of Hypotenuse
				int centerY = (leftY + rightY)>>1; // Compute Y coord...

				if ( node < (1<<VARIANCE_DEPTH) )
				{
					// Extremely slow distance metric (sqrt is used).
					// Replace this with a faster one!
					float distance = 1.0f + sqrt(BREATHE::MATH::sqrf((float)centerX - cHeightmap::pFrustum->eye.x) +
						BREATHE::MATH::sqrf((float)centerY - cHeightmap::pFrustum->eye.z) );
					
					// Egads!  A division too?  What's this world coming to!
					// This should also be replaced with a faster operation.
					TriVariance = ((float)m_CurrentVariance[node] * MAP_SIZE * 2)/distance;	// Take both distance and variance into consideration
				}

				if ( (node >= (1<<VARIANCE_DEPTH)) ||	// IF we do not have variance info for this node, then we must have gotten here by splitting, so continue down to the lowest level.
					(TriVariance > cHeightmap::gFrameVariance))	// OR if we are not below the variance tree, test for variance.
				{
					Split(tri);														// Split this triangle.
					
					if (tri->LeftChild &&											// If this triangle was split, try to split it's children as well.
						((abs(leftX - rightX) >= 3) || (abs(leftY - rightY) >= 3)))	// Tessellate all the way down to one vertex per height field entry
					{
						RecursTessellate( tri->LeftChild,   apexX,  apexY, leftX, leftY, centerX, centerY,    node<<1  );
						RecursTessellate( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY, 1+(node<<1) );
					}
				}
			}

			// ---------------------------------------------------------------------
			// Render the tree.  Simple no-fan method.
			//
			void cHeightmapPatch::RecursRender( cTriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY )
			{
				if ( tri->LeftChild )					// All non-leaf nodes have both children, so just check for one
				{
					int centerX = (leftX + rightX)>>1;	// Compute X coordinate of center of Hypotenuse
					int centerY = (leftY + rightY)>>1;	// Compute Y coord...

					RecursRender( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY );
					RecursRender( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY );
				}
				else									// A leaf node!  Output a triangle to be rendered.
				{
					MATH::cVec3 p[4];

					p[0]=MATH::cVec3(cHeightmap::scale*((float)leftX + m_WorldX),	cHeightmap::scale*(float)m_HeightMap[(leftY *MAP_SIZE)+leftX],	cHeightmap::scale*((float)leftY + m_WorldY));
					p[1]=MATH::cVec3(cHeightmap::scale*((float)rightX + m_WorldX), cHeightmap::scale*(float)m_HeightMap[(rightY*MAP_SIZE)+rightX],	cHeightmap::scale*((float)rightY + m_WorldY));
					p[2]=MATH::cVec3(cHeightmap::scale*((float)apexX + m_WorldX),	cHeightmap::scale*(float)m_HeightMap[(apexY *MAP_SIZE)+apexX],	cHeightmap::scale*((float)apexY + m_WorldY));
					
					MATH::cPlane plane;
					plane.SetFromPoints(p[0],p[1],p[2]);

					if((p[0] - cHeightmap::pFrustum->eye).DotProduct(plane.normal)>0.0f)
						return;

					if(cHeightmap::renderFog)
					{
						//SetFogCoord(g_FogDepth, p[0].y);
						//SetTextureCoord(p[0].x, p[0].z);
						glVertex3f(p[0].x, p[0].y, p[0].z);

						//SetFogCoord(g_FogDepth, p[1].y);
						//SetTextureCoord(p[1].x, p[1].z);
						glVertex3f(p[1].x, p[1].y, p[1].z);

						//SetFogCoord(g_FogDepth, p[2].y);
						//SetTextureCoord(p[2].x, p[2].z);
						glVertex3f(p[2].x, p[2].y, p[2].z);
					}
					else
					{
						//SetTextureCoord(p[0].x, p[0].z);
						glVertex3f(p[0].x, p[0].y, p[0].z);

						//SetTextureCoord(p[1].x, p[1].z);
						glVertex3f(p[1].x, p[1].y, p[1].z);

						//SetTextureCoord(p[2].x, p[2].z);
						glVertex3f(p[2].x, p[2].y, p[2].z);
					}

					// Actual number of rendered triangles...
					//level.uiTriangles++;
				}
			}

			// ---------------------------------------------------------------------
			// Computes Variance over the entire tree.  Does not examine node relationships.
			//
			unsigned char cHeightmapPatch::RecursComputeVariance( int leftX,  int leftY,  unsigned char leftZ,
															int rightX, int rightY, unsigned char rightZ,
														int apexX,  int apexY,  unsigned char apexZ,
														int node)
			{
				//        /|\
				//      /  |  \
				//    /    |    \
				//  /      |      \
				//  ~~~~~~~*~~~~~~~  <-- Compute the X and Y coordinates of '*'
				//
				int centerX = (leftX + rightX) >>1;		// Compute X coordinate of center of Hypotenuse
				int centerY = (leftY + rightY) >>1;		// Compute Y coord...
				unsigned char myVariance=0;

				// Get the height value at the middle of the Hypotenuse
				unsigned char centerZ  = m_HeightMap[(centerY * MAP_SIZE) + centerX];

				// Variance of this triangle is the actual height at it's hypotenuse midpoint minus the interpolated height.
				// Use values passed on the stack instead of re-accessing the Height Field.
				myVariance = abs((int)centerZ - (((int)leftZ + (int)rightZ)>>1));

				// Since we're after speed and not perfect representations,
				//    only calculate variance down to an 8x8 block
				if ( (abs(leftX - rightX) >= 8) ||
					(abs(leftY - rightY) >= 8) )
				{
					// Final Variance for this node is the max of it's own variance and that of it's children.
					myVariance = min( myVariance, RecursComputeVariance( apexX,   apexY,  apexZ, leftX, leftY, leftZ, centerX, centerY, centerZ,    node<<1 ) );
					myVariance = max( myVariance, RecursComputeVariance( rightX, rightY, rightZ, apexX, apexY, apexZ, centerX, centerY, centerZ, 1+(node<<1)) );
				}

				// Store the final variance for this node.  Note Variance is never zero.
				if (node < (1<<VARIANCE_DEPTH))
					m_CurrentVariance[node] = 1 + myVariance;

				return myVariance;
			}

			// -------------------------------------------------------------------------------------------------
			//	PATCH CLASS
			// -------------------------------------------------------------------------------------------------

			// ---------------------------------------------------------------------
			// Initialize a patch.
			//
			void cHeightmapPatch::Init( int heightX, int heightY, int worldX, int worldY, unsigned char *hMap )
			{
				// Clear all the relationships
				m_BaseLeft.RightNeighbor = m_BaseLeft.LeftNeighbor = m_BaseRight.RightNeighbor = m_BaseRight.LeftNeighbor =
				m_BaseLeft.LeftChild = m_BaseLeft.RightChild = m_BaseRight.LeftChild = m_BaseLeft.LeftChild = NULL;

				// Attach the two m_Base triangles together
				m_BaseLeft.BaseNeighbor = &m_BaseRight;
				m_BaseRight.BaseNeighbor = &m_BaseLeft;

				// Store Patch offsets for the world and heightmap.
				m_WorldX = worldX;
				m_WorldY = worldY;

				// Store pointer to first byte of the height data for this patch.
				m_HeightMap = &hMap[heightY * MAP_SIZE + heightX];

				// Initialize flags
				m_VarianceDirty = 1;
				m_isVisible = 0;
			}

			// ---------------------------------------------------------------------
			// Reset the patch.
			//
			void cHeightmapPatch::Reset()
			{
				// Assume patch is not visible.
				m_isVisible = 0;

				// Reset the important relationships
				m_BaseLeft.LeftChild = m_BaseLeft.RightChild = m_BaseRight.LeftChild = m_BaseLeft.LeftChild = NULL;

				// Attach the two m_Base triangles together
				m_BaseLeft.BaseNeighbor = &m_BaseRight;
				m_BaseRight.BaseNeighbor = &m_BaseLeft;

				// Clear the other relationships.
				m_BaseLeft.RightNeighbor = m_BaseLeft.LeftNeighbor = m_BaseRight.RightNeighbor = m_BaseRight.LeftNeighbor = NULL;
			}

			// ---------------------------------------------------------------------
			// Compute the variance tree for each of the Binary Triangles in this patch.
			//
			void cHeightmapPatch::ComputeVariance()
			{
				// Compute variance on each of the base triangles...

				m_CurrentVariance = m_VarianceLeft;
				RecursComputeVariance(	0,          PATCH_SIZE, m_HeightMap[PATCH_SIZE * MAP_SIZE],
										PATCH_SIZE, 0,          m_HeightMap[PATCH_SIZE],
										0,          0,          m_HeightMap[0],
										1);

				m_CurrentVariance = m_VarianceRight;
				RecursComputeVariance(	PATCH_SIZE, 0,          m_HeightMap[ PATCH_SIZE],
										0,          PATCH_SIZE, m_HeightMap[ PATCH_SIZE * MAP_SIZE],
										PATCH_SIZE, PATCH_SIZE, m_HeightMap[(PATCH_SIZE * MAP_SIZE) + PATCH_SIZE],
										1);

				// Clear the dirty flag for this patch
				m_VarianceDirty = 0;
			}

			// ---------------------------------------------------------------------
			// Discover the orientation of a triangle's points:
			//
			// Taken from "Programming Principles in Computer Graphics", L. Ammeraal (Wiley)
			//
			inline int orientation( int pX, int pY, int qX, int qY, int rX, int rY )
			{
				int aX, aY, bX, bY;
				float d;

				aX = qX - pX;
				aY = qY - pY;

				bX = rX - pX;
				bY = rY - pY;

				d = (float)aX * (float)bY - (float)aY * (float)bX;
				return (d < 0) ? (-1) : (d > 0);
			}

			// ---------------------------------------------------------------------
			// Set patch's visibility flag.
			//
			void cHeightmapPatch::SetVisibility( int eyeX, int eyeY, int leftX, int leftY, int rightX, int rightY )
			{
				// Get patch's center point
				int patchCenterX = (int)cHeightmap::scale*(m_WorldX + (PATCH_SIZE>>1));
				int patchCenterY = (int)cHeightmap::scale*(m_WorldY + (PATCH_SIZE>>1));
				
				// Set visibility flag (orientation of both triangles must be counter clockwise)
				m_isVisible = (orientation( eyeX,  eyeY,  rightX, rightY, patchCenterX, patchCenterY ) < 0) &&
								(orientation( leftX, leftY, eyeX,   eyeY,   patchCenterX, patchCenterY ) < 0);
			}

			// ---------------------------------------------------------------------
			// Create an approximate mesh.
			//
			void cHeightmapPatch::Tessellate()
			{
				// Split each of the base triangles
				m_CurrentVariance = m_VarianceLeft;
				RecursTessellate (	&m_BaseLeft,
									m_WorldX,				m_WorldY+PATCH_SIZE,
									m_WorldX+PATCH_SIZE,	m_WorldY,
									m_WorldX,				m_WorldY,
									1 );
								
				m_CurrentVariance = m_VarianceRight;
				RecursTessellate(	&m_BaseRight,
									m_WorldX+PATCH_SIZE,	m_WorldY,
									m_WorldX,				m_WorldY+PATCH_SIZE,
									m_WorldX+PATCH_SIZE,	m_WorldY+PATCH_SIZE,
									1 );
			}


			void cHeightmapPatch::RecursRenderDebug( cTriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY )
			{
				if ( tri->LeftChild )					// All non-leaf nodes have both children, so just check for one
				{
					int centerX = (leftX + rightX)>>1;	// Compute X coordinate of center of Hypotenuse
					int centerY = (leftY + rightY)>>1;	// Compute Y coord...

					RecursRenderDebug( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY );
					RecursRenderDebug( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY );
				}
				else									// A leaf node!  Output a triangle to be rendered.
				{
					MATH::cVec3 p[3];

					p[0]=MATH::cVec3(cHeightmap::scale*((float)leftX + m_WorldX),	cHeightmap::scale*(float)m_HeightMap[(leftY *MAP_SIZE)+leftX],	cHeightmap::scale*((float)leftY + m_WorldY));
					p[1]=MATH::cVec3(cHeightmap::scale*((float)rightX + m_WorldX), cHeightmap::scale*(float)m_HeightMap[(rightY*MAP_SIZE)+rightX],	cHeightmap::scale*((float)rightY + m_WorldY));
					p[2]=MATH::cVec3(cHeightmap::scale*((float)apexX + m_WorldX),	cHeightmap::scale*(float)m_HeightMap[(apexY *MAP_SIZE)+apexX],	cHeightmap::scale*((float)apexY + m_WorldY));
					
					MATH::cPlane plane;
					plane.SetFromPoints(p[0],p[1],p[2]);

					if((p[0] - cHeightmap::pFrustum->eye).DotProduct(plane.normal)>0.0f)
						return;

					float length=10.0f;
					glEnd();

					glBegin(GL_LINES);
						glVertex3f(p[3].x, p[3].y, p[3].z);
						glVertex3f(p[3].x+plane.normal.x*length, 
							p[3].y+plane.normal.y*length, 
							p[3].z+plane.normal.z*length);
					glEnd();

					glBegin( GL_TRIANGLES);

					if(cHeightmap::renderFog)
					{
						//unrolled for speed
						// We want to render triangle strips
						//glBegin( GL_TRIANGLES);			

							//SetFogCoord(g_FogDepth, p[0].y);
							//SetTextureCoord(p[0].x, p[0].z);
							glVertex3f(p[0].x, p[0].y, p[0].z);

							//SetFogCoord(g_FogDepth, p[1].y);
							//SetTextureCoord(p[1].x, p[1].z);
							glVertex3f(p[1].x, p[1].y, p[1].z);

							//SetFogCoord(g_FogDepth, p[2].y);
							//SetTextureCoord(p[2].x, p[2].z);
							glVertex3f(p[2].x, p[2].y, p[2].z);
						
						// Stop rendering triangle strips
						//glEnd();
					}
					else
					{
						//unrolled for speed
						//SetTextureCoord(p[0].x, p[0].z);
						glVertex3f(p[0].x, p[0].y, p[0].z);

						//SetTextureCoord(p[1].x, p[1].z);
						glVertex3f(p[1].x, p[1].y, p[1].z);

						//SetTextureCoord(p[2].x, p[2].z);
						glVertex3f(p[2].x, p[2].y, p[2].z);
					}

					// Actual number of rendered triangles...
					//cHeightmap::renderedTriangles++;
				}
			}

			void cHeightmapPatch::RecursAddTriangles( cTriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY )
			{
				if ( tri->LeftChild )					// All non-leaf nodes have both children, so just check for one
				{
					int centerX = (leftX + rightX)>>1;	// Compute X coordinate of center of Hypotenuse
					int centerY = (leftY + rightY)>>1;	// Compute Y coord...

					RecursAddTriangles( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY );
					RecursAddTriangles( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY );
				}
				else									// A leaf node!  Output a triangle to be rendered.
				{
					MATH::cVec3 p[3];

					p[0]=MATH::cVec3(cHeightmap::scale*((float)leftX + m_WorldX),	cHeightmap::scale*(float)m_HeightMap[(leftY *MAP_SIZE)+leftX],	cHeightmap::scale*((float)leftY + m_WorldY));
					p[1]=MATH::cVec3(cHeightmap::scale*((float)rightX + m_WorldX), cHeightmap::scale*(float)m_HeightMap[(rightY*MAP_SIZE)+rightX],	cHeightmap::scale*((float)rightY + m_WorldY));
					p[2]=MATH::cVec3(cHeightmap::scale*((float)apexX + m_WorldX),	cHeightmap::scale*(float)m_HeightMap[(apexY *MAP_SIZE)+apexX],	cHeightmap::scale*((float)apexY + m_WorldY));

					triangle.point0.x=p[0].x;
					triangle.point0.y=p[0].y;
					triangle.point0.z=p[0].z;
					triangle.point0.ux=p[0].x;
					triangle.point0.fy=p[0].y;
					triangle.point0.uz=p[0].z;

					triangle.point1.x=p[1].x;
					triangle.point1.y=p[1].y;
					triangle.point1.z=p[1].z;
					triangle.point1.ux=p[1].x;
					triangle.point1.fy=p[1].y;
					triangle.point1.uz=p[1].z;
					
					triangle.point2.x=p[2].x;
					triangle.point2.y=p[2].y;
					triangle.point2.z=p[2].z;
					triangle.point2.ux=p[2].x;
					triangle.point2.fy=p[2].y;
					triangle.point2.uz=p[2].z;

					//level.triangles.push_back(&triangle);
					//level.uiTriangles++;
				}
			}

			void cHeightmapPatch::AddTriangles()
			{
				RecursAddTriangles(	&m_BaseLeft,
					0,				PATCH_SIZE,
					PATCH_SIZE,		0,
					0,				0);
				
				RecursAddTriangles(	&m_BaseRight,
					PATCH_SIZE,		0,
					0,				PATCH_SIZE,
					PATCH_SIZE,		PATCH_SIZE);
			}

			// ---------------------------------------------------------------------
			// Render the mesh.
			//
			void cHeightmapPatch::Render()
			{		
				RecursRender (	&m_BaseLeft,
					0,				PATCH_SIZE,
					PATCH_SIZE,		0,
					0,				0);
				
				RecursRender(	&m_BaseRight,
					PATCH_SIZE,		0,
					0,				PATCH_SIZE,
					PATCH_SIZE,		PATCH_SIZE);
			}

			void cHeightmapPatch::RenderDebug()
			{
				// Store old matrix
				//glPushMatrix();
				
				// Translate the patch to the proper world coordinates
				//glTranslatef( (GLfloat)m_WorldX, 0, (GLfloat)m_WorldY );
				//glBegin(GL_TRIANGLES);
					
					RecursRenderDebug(	&m_BaseLeft,
						0,				PATCH_SIZE,
						PATCH_SIZE,		0,
						0,				0);
					
					RecursRenderDebug(	&m_BaseRight,
						PATCH_SIZE,		0,
						0,				PATCH_SIZE,
						PATCH_SIZE,		PATCH_SIZE);
				
				//glEnd();
				
				// Restore the matrix
				//glPopMatrix();
			}

			void SetTextureCoord(float x, float z)
			{
				// Find the (u, v) coordinate for the current vertex
				float u = cHeightmap::oneOverScale*  (float)x / (float)cHeightmap::width;
				float v = cHeightmap::oneOverScale* -(float)z / (float)cHeightmap::height;
				
				// Give OpenGL the current terrain texture coordinate for our height map
				//glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u, v);

				// Give OpenGL the current detail texture coordinate for our height map
				//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, u, v);

				// Give OpenGL the current detail texture coordinate for our height map
				//glMultiTexCoord2fARB(GL_TEXTURE2_ARB, u, v);
			}


			/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

			///////////////////////////////// SET FOG COORD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
			/////
			/////	This sets the volumetric fog for the current vertex with the desired depth
			/////
			///////////////////////////////// SET FOG COORD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

			void SetFogCoord(float depth, float height)
			{
				// This function takes the depth of the fog, as well as the height
				// of the current vertex.  If the height is greater than the depth, there
				// is no fog applied to it (0), but if it's below the depth, then we
				// calculate the fog value that should be applied to it.  Since the higher
				// the number passed into glFogCoordfEXT() produces more fog, we need to
				// subtract the depth from the height, then negate that value to switch
				// the ratio from 0 to the depth around.  Otherwise it would have more
				// fog on the top of the fog volume than the bottom of it.

				float fogY = 0;

				// Check if the height of this vertex is greater than the depth (needs no fog)
				if(height > depth)
					fogY = 0;
				// Otherwise, calculate the fog depth for the current vertex
				else
					fogY = -(height - depth);

				// Assign the fog coordinate for this vertex using our extension function pointer
				//glFogCoordfEXT(fogY);
			}

			void cTriangle::Render()
			{
				//SetTextureCoord(point0.ux, point0.uz);
				glVertex3f(point0.x, point0.y, point0.z);

				//SetTextureCoord(point1.ux, point1.uz);
				glVertex3f(point1.x, point1.y, point1.z);
				
				//SetTextureCoord(point2.ux, point2.uz);
				glVertex3f(point2.x, point2.y, point2.z);
			}

			void cTriangle::RenderFog()
			{
				//SetFogCoord(g_FogDepth, point0.fy);
				//SetTextureCoord(point0.ux, point0.uz);
				glVertex3f(point0.x, point0.y, point0.z);

				//SetFogCoord(g_FogDepth, point1.fy);
				//SetTextureCoord(point1.ux, point1.uz);
				glVertex3f(point1.x, point1.y, point1.z);
				
				//SetFogCoord(g_FogDepth, point2.fy);
				//SetTextureCoord(point2.ux, point2.uz);
				glVertex3f(point2.x, point2.y, point2.z);
			}*/
		}
	}
}

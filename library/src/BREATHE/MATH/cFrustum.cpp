#include <math.h>

// Breathe
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

namespace BREATHE
{
	namespace MATH
	{
		// This is the index in our selection buffer that has the closet object ID clicked
		#define FIRST_OBJECT_ID  3								

		// We create an enum of the sides so we don't have to call each side 0 or 1.
		// This way it makes it more understandable and readable when dealing with frustum sides.
		enum FrustumSide
		{
			RIGHT	= 0,		// The RIGHT side of the frustum
			LEFT	= 1,		// The LEFT	 side of the frustum
			BOTTOM	= 2,		// The BOTTOM side of the frustum
			TOP		= 3,		// The TOP side of the frustum
			BACK	= 4,		// The BACK	side of the frustum
			FRONT	= 5			// The FRONT side of the frustum
		}; 

		// Like above, instead of saying a number for the ABC and D of the plane, we
		// want to be more descriptive.
		enum PlaneData
		{
			A = 0,				// The X value of the plane's normal
			B = 1,				// The Y value of the plane's normal
			C = 2,				// The Z value of the plane's normal
			D = 3				// The distance the plane is from the origin
		};

		///////////////////////////////// NORMALIZE PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
		/////
		/////	This normalizes a plane (A side) from a given frustum.
		/////
		///////////////////////////////// NORMALIZE PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

		void cFrustum::NormalizePlane(float frustum[6][4], int side)
		{
			// Here we calculate the magnitude of the normal to the plane (point A B C)
			// Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
			// To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
			float magnitude = sqrtf(	frustum[side][A] * frustum[side][A] + 
																							frustum[side][B] * frustum[side][B] + 
																							frustum[side][C] * frustum[side][C] );

			// Then we divide the plane's values by it's magnitude.
			// This makes it easier to work with.
			frustum[side][A] /= magnitude;
			frustum[side][B] /= magnitude;
			frustum[side][C] /= magnitude;
			frustum[side][D] /= magnitude; 
		}


		//-----------------------------------------------------------------------------
		// Name : updateViewMatrix() 
		// Desc : Builds a view matrix suitable for OpenGL.
		//
		// Here's what the final view matrix should look like:
		//
		//  |  rx   ry   rz  -(r.e) |
		//  |  ux   uy   uz  -(u.e) |
		//  | -lx  -ly  -lz   (l.e) |
		//  |   0    0    0     1   |
		//
		// Where r = Right vector
		//       u = Up vector
		//       l = Look vector
		//       e = Eye position in world space
		//       . = Dot-product operation
		//
		//-----------------------------------------------------------------------------

		void cFrustum::update(cVec3 & newLook, cVec3 & newEye, cVec3 & newUp)
		{
			look=newLook;
			look.Normalize();

			eye=newEye;
			
			up=newUp;
			
			right=look.CrossProduct(up);
			right.Normalize();

			up=right.CrossProduct(look);
			up.Normalize();

			modl[0] =  right.x;
			modl[1] =  up.x;
			modl[2] = -look.x;
			modl[3] =  0.0f;

			modl[4] =  right.y;
			modl[5] =  up.y;
			modl[6] = -look.y;
			modl[7] =  0.0f;

			modl[8]  =  right.z;
			modl[9]  =  up.z;
			modl[10] = -look.z;
			modl[11] =  0.0f;

			modl[12] = -right.DotProduct(eye);
			modl[13] = -up.DotProduct(eye);
			modl[14] =  look.DotProduct(eye);
			modl[15] =  1.0f;



			proj[0]		= 1.29904f;
			proj[1]		=	0.000000f;
			proj[2]		=	0.000000f;
			proj[3]		=	0.000000f;
			proj[4]		=	0.000000f;
			proj[5]		=	1.73205f;
			proj[6]		=	0.000000f;
			proj[7]		=	0.000000f;
			proj[8]		=	0.000000f;
			proj[9]		=	0.000000f;
			proj[10]	=	-1.00200f;
			proj[11]	=	-1.00000f;
			proj[12]	=	0.000000f;
			proj[13]	=	0.000000f;
			proj[14]	=	-2.00200f;
			proj[15]	=	0.000000f;

			// Now that we have our modelview and projection matrix, if we combine these 2 matrices,
			// it will give us our clipping planes.  To combine 2 matrices, we multiply them.

 			clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
			clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
			clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
			clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

			clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
			clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
			clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
			clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

			clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
			clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
			clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
			clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

			clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
			clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
			clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
			clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];
			
			// Now we actually want to get the sides of the frustum.  To do this we take
			// the clipping planes we received above and extract the sides from them.

			// This will extract the RIGHT side of the frustum
			m_Frustum[RIGHT][A] = clip[ 3] - clip[ 0];
			m_Frustum[RIGHT][B] = clip[ 7] - clip[ 4];
			m_Frustum[RIGHT][C] = clip[11] - clip[ 8];
			m_Frustum[RIGHT][D] = clip[15] - clip[12];

			// Now that we have a normal (A,B,C) and a distance (D) to the plane,
			// we want to normalize that normal and distance.

			// Normalize the RIGHT side
			NormalizePlane(m_Frustum, RIGHT);

			// This will extract the LEFT side of the frustum
			m_Frustum[LEFT][A] = clip[ 3] + clip[ 0];
			m_Frustum[LEFT][B] = clip[ 7] + clip[ 4];
			m_Frustum[LEFT][C] = clip[11] + clip[ 8];
			m_Frustum[LEFT][D] = clip[15] + clip[12];

			// Normalize the LEFT side
			NormalizePlane(m_Frustum, LEFT);

			// This will extract the BOTTOM side of the frustum
			m_Frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
			m_Frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
			m_Frustum[BOTTOM][C] = clip[11] + clip[ 9];
			m_Frustum[BOTTOM][D] = clip[15] + clip[13];

			// Normalize the BOTTOM side
			NormalizePlane(m_Frustum, BOTTOM);

			// This will extract the TOP side of the frustum
			m_Frustum[TOP][A] = clip[ 3] - clip[ 1];
			m_Frustum[TOP][B] = clip[ 7] - clip[ 5];
			m_Frustum[TOP][C] = clip[11] - clip[ 9];
			m_Frustum[TOP][D] = clip[15] - clip[13];

			// Normalize the TOP side
			NormalizePlane(m_Frustum, TOP);

			// This will extract the BACK side of the frustum
			m_Frustum[BACK][A] = clip[ 3] - clip[ 2];
			m_Frustum[BACK][B] = clip[ 7] - clip[ 6];
			m_Frustum[BACK][C] = clip[11] - clip[10];
			m_Frustum[BACK][D] = clip[15] - clip[14];

			// Normalize the BACK side
			NormalizePlane(m_Frustum, BACK);

			// This will extract the FRONT side of the frustum
			m_Frustum[FRONT][A] = clip[ 3] + clip[ 2];
			m_Frustum[FRONT][B] = clip[ 7] + clip[ 6];
			m_Frustum[FRONT][C] = clip[11] + clip[10];
			m_Frustum[FRONT][D] = clip[15] + clip[14];

			// Normalize the FRONT side
			NormalizePlane(m_Frustum, FRONT);
		}

		// The code below will allow us to make checks within the frustum.  For example,
		// if we want to see if a point, a sphere, or a cube lies inside of the frustum.
		// Because all of our planes point INWARDS (The normals are all pointing inside the frustum)
		// we then can assume that if a point is in FRONT of all of the planes, it's inside.

		///////////////////////////////// POINT IN cFrustum \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
		/////
		/////	This determines if a point is inside of the frustum
		/////
		///////////////////////////////// POINT IN cFrustum \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

		bool cFrustum::PointInFrustum( float x, float y, float z )
		{
			// Go through all the sides of the frustum
			for(int i = 0; i < 6; i++ )
			{
				// Calculate the plane equation and check if the point is behind a side of the frustum
				if(m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= 0)
				{
					// The point was behind a side, so it ISN'T in the frustum
					return false;
				}
			}

			// The point was inside of the frustum (In front of ALL the sides of the frustum)
			return true;
		}


		///////////////////////////////// SPHERE IN cFrustum \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
		/////
		/////	This determines if a sphere is inside of our frustum by it's center and radius.
		/////
		///////////////////////////////// SPHERE IN cFrustum \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

		bool cFrustum::SphereInFrustum( float x, float y, float z, float radius )
		{
			// Go through all the sides of the frustum
			for(int i = 0; i < 6; i++ )	
			{
				// If the center of the sphere is farther away from the plane than the radius
				if( m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= -radius )
				{
					// The distance was greater than the radius so the sphere is outside of the frustum
					return false;
				}
			}
			
			// The sphere was inside of the frustum!
			return true;
		}


		///////////////////////////////// CUBE IN cFrustum \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
		/////
		/////	This determines if a cube is in or around our frustum by it's center and 1/2 it's length
		/////
		///////////////////////////////// CUBE IN cFrustum \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

		bool cFrustum::CubeInFrustum( float x, float y, float z, float size )
		{
			// Basically, what is going on is, that we are given the center of the cube,
			// and half the length.  Think of it like a radius.  Then we checking each point
			// in the cube and seeing if it is inside the frustum.  If a point is found in front
			// of a side, then we skip to the next side.  If we get to a plane that does NOT have
			// a point in front of it, then it will return false.

			// *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
			// This happens when all the corners of the bounding box are not behind any one plane.
			// This is rare and shouldn't effect the overall rendering speed.

			for(int i = 0; i < 6; i++ )
			{
				if(m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] > 0)
					continue;
				if(m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] > 0)
					continue;
				if(m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] > 0)
					continue;
				if(m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] > 0)
					continue;
				if(m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] > 0)
					continue;
				if(m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] > 0)
					continue;
				if(m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] > 0)
					continue;
				if(m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] > 0)
					continue;

				// If we get here, it isn't in the frustum
				return false;
			}

			return true;
		}
	}
}

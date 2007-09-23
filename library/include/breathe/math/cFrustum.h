#ifndef CFRUSTUM_H
#define CFRUSTUM_H

namespace BREATHE
{
	namespace MATH
	{
		class cFrustum
		{
		protected:
			cMat4 proj;								// This will hold our projection matrix
			cMat4 modl;								// This will hold our modelview matrix
			cMat4 clip;								// This will hold the clipping planes
			
			// This holds the A B C and D values for each side of our cFrustum.
			float m_Frustum[6][4];

		public:
			cFrustum();
			~cFrustum();

			void Update();


			void NormalizePlane(float frustum[6][4], int side);

			// Call this every time the camera moves to update the cFrustum
			//void update(cVec3 & newLook, cVec3 & newEye, cVec3 & newUp);

			// This takes a 3D point and returns true if it's inside of the cFrustum
			bool PointInFrustum(float x, float y, float z);

			// This takes a 3D point and a radius and returns true if the sphere is inside of the cFrustum
			bool SphereInFrustum(float x, float y, float z, float radius);

			// This takes the center and half the length of the cube.
			bool CubeInFrustum( float x, float y, float z, float size );

			float fov;
			
			cVec3 target, eye, up, right;
			cVec3 targetIdeal, eyeIdeal, upIdeal, rightIdeal;
			cMat4 m;
		};
	}
}

#endif //CFRUSTUM_H

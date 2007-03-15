#ifndef CCAMERA_H
#define CCAMERA_H

namespace BREATHE
{
	namespace RENDER
	{
		class cCamera
		{
		public:
			cCamera();
			~cCamera();

			void Update();

			MATH::cVec3 target, eye, up;
			MATH::cVec3 targetIdeal, eyeIdeal, upIdeal;
			MATH::cMat4 m;
		};
	}
}

#endif //CCAMERA_H

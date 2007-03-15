#ifndef CBASE_H
#define CBASE_H

//Base Objects to derive game types from

namespace BREATHE
{
	//This can be used for generic things such as the audio, renderer, game etc.
	class cUpdateable
	{
		virtual void Update(float fTime)=0;
	};

	//This is for in game objects that have a physical presence in the world such as the player, 
	//vehicles, level pieces etc.
	class cObject : virtual public cUpdateable
	{
	public:
		float fRadius; //For basic collision detection
		float fWidth, fLength, fHeight;

		float fWeight;
		
		//Position and rotation
		MATH::cVec3 p;
		float r[12];

		MATH::cVec3 v;

		//Position and rotation matrix
		MATH::cMat4 m;

		cObject();
	};
}

#endif //CBASE_H

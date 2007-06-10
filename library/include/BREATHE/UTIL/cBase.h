#ifndef CBASE_H
#define CBASE_H

// Base Objects to derive game types from

namespace BREATHE
{
	// This can be used for generic things such as the audio, renderer, game etc.
	// Pure virtual
	class cUpdateable
	{
	public:
		virtual void Update(float fCurrentTime)=0;
	};

	// Pure virtual
	class cRenderable : protected cUpdateable
	{
	public:
		virtual unsigned int Render()=0;
	};

  // General class to facilitate encapsulation of an object or 
	// collection of objects within a bounding sphere.  
	class cObjectSphere
	{
	public:
		cObjectSphere();

		void SetDimensions(float fRadius);

		virtual bool Collide(cObjectSphere& obj);
		virtual float GetDistance(cObjectSphere& obj);

		// Position and size
		MATH::cVec3 p;
		float fRadius;
	};

	// This is for in game objects that have a physical presence in the world such as the player, 
	// vehicles, level pieces etc.
	// Instead of having a separate class for each bounding type,
	// we list them all here and have a different collide for each one
	// Pure virtual
	class cObject : virtual public cUpdateable, public cObjectSphere
	{
	public:
		cObject();

		// Box for very general collision and culling
		float fWidth, fLength, fHeight;
		float fWeight;
		
		//Position and rotation
		//float r[12];

		MATH::cVec3 v;

		//Position and rotation matrix
		MATH::cMat4 m;


		void SetDimensions(float fWidth, float fLength, float fHeight);

		float GetDistance(cObject& obj);
		bool Collide(cObject& obj);
	};
}

#endif //CBASE_H

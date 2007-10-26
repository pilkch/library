#ifndef CBASE_H
#define CBASE_H

// Base Objects to derive game types from

namespace breathe
{
  // General class to facilitate encapsulation of an object or 
	// collection of objects within a bounding sphere.  
	class cObjectSphere
	{
	public:
		cObjectSphere();

		void SetDimensions(float fRadius);

		virtual bool Collide(cObjectSphere& obj);
		virtual float GetDistance(cObjectSphere& obj) const;

		// Position and size
		math::cVec3 p;
		float fRadius;
	};

	
	// This can be used for generic things such as the audio, renderer, game etc.
	// Pure virtual
	class cUpdateable
	{
	public:
		virtual void Update(sampletime_t currentTime)=0;
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

		math::cVec3 v;

		//Position and rotation matrix
		math::cMat4 m;

		void SetDimensions(float fWidth, float fLength, float fHeight);

		float GetDistance(cObject& obj) const;
		bool Collide(cObject& obj);
	};

	typedef cObject cRenderable;

	class cScenegraphNode : public virtual cObject
	{
	public:
		math::cMat4 mOffset;
	};

	class cOctreeNode : public virtual cObject
	{
	public:
		
	};

	class cNode : public cScenegraphNode, public cOctreeNode
	{
	public:
		
	};

	/*app
		scenegraph
	cull
		octree
	sort
		sort states (texture, shader)
		transparent
	draw



1) I parse my scenegraph / object list / whatever to determine what is visible or not. 
I send all the visible data to a renderer. This renderer is only a stack which store a 
structures containing all the data needed for rendering : geometry, textures, materials, shaders, etc.
2) this structure will implement the ==, <, > etc. operator to sort by (in this order, 
but that can change depending of the results ^^) shader, textures, materials.
3) After the visibility pass, the renderer is filled with all the visible node, and I 
simply qsort my stack (or any other sorting algorithms, I'll have to search on that subject) 
and then render the sorted data (using a piece of code similar to what you wrote)

   operator < (SData data)
   {
      if (_pEffect < data._pEffect)
         return(true);
      if (_pEffect == data._pEffect && _pTexture < data._pTexture)
         return(true);
      if (_pEffect == data._pEffect && _pTexture == data._pTexture && otherproperty < data.otherproperty)
         return(true);
 
     // and so on
     // ...

      return(false);
   }

Consider shader LOD, in the distance revert to fixed pipe line at the same point 
(different point for smoothness?) as changing to mesh lower LOD.  
Add butterflies fluttering in the grass randomly. 









- Rendering engine
- Sound engine
- Some kind of AI system

- player control
- A camera
- Collision and physics
- A way to describe game objects
- A file loader, or a collection of file loaders

Now start to add in some details inside the boxes of the subsstems...


- Rendering engine
    - Texture manager (for loading and storing textures)
    - A way to handle, store and draw meshes
    - A way to draw the world (might be the same as the way you draw meshes)
    - An animation system

    - A frustum
    - A particle system
    - A way to draw 2D stuff and text for the HUD and menus
- Sound engine
    - A way to stream music and speech
    - A manager for smaller sounds to be loaded and played when you want them

- Some kind of AI system
    - A generic state machine
    - A way to create states and fit them together
    - Pathfinding
- player control
    - Some kind of raw input handler (keyboard? Mouse? Joypad?)

    - A way to turn the raw input into game actions
- Collision and physics
   - A way to represent different collidable shapes (spheres, boxes, cylinders etc)
   - A way to collide with the world (BSP? Octree?)

   - A way for objects to collide with each other
   - A way to work out what's happened when collisions occur
- A way to describe game objects
   - A structure you can fill in a variety of different ways to represent different things

     (Remember code reuse is the key here, so the game object shouldn't
      do much in itself, just draw together bits of other things)
    - A hierarchy? A group of hierarchies? Something totally different?

    - A manager to look after all the entities
- A file loader, or a collection of file loaders
    - A common interface?
    - A collection of different loaders, one for textures, one for meshes,
     one for the world maybe, one for sounds, one for data files...

    - A memory manager


You typically want objects implementing your Viewable interface to go into some scene graph (like an octree). 
Then, you use a Visitor to visit all Viewables that intersect the viewing frustum. 
The protocol for Viewable is typically to return a description of some sort (i e, group of geometry, 
material, and pose/matrix state) which the renderer uses to actually render the thing. 
The reason you don't want objects calling renderer->setTexture() and similar functions directly is 
that then, if you change your rendering to require multi-passing (for shadows, reflection, etc)
then you have to update ALL the objects, if the objects do the rendering. 
Having objects describe themselves with multiples of geometry-material-state is much more flexible,
as you can easily throw that at mirroring, cube mapping, shadow maps, shadow volumes, cel shading, 
and whatever else you care about implementing.*/
}

#endif //CBASE_H

#ifndef _MD3_H
#define _MD3_H

#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>

#include <breathe/breathe.h>

#include <breathe/render/cContext.h>
#include <breathe/render/model/cAnimation.h>

namespace breathe
{
  namespace character
  {
    const size_t MAX_TEXTURES = 30; // How many could an md3 possibly have?

    // This file stores all of our structures and classes (besides the modular model ones in main.h)
    // in order to read in and display a Quake3 character.  The file format is of type
    // .MD3 and comes in many different files for each main body part section.  We convert
    // these Quake3 structures to our own structures in md3.cpp so that we are not dependant
    // on their model data structures.  You can do what ever you want, but I like mine :P :)
    // These defines are used to pass into GetModel(), which is member function of CModelMD3

    #define kLower  0                       // This stores the ID for the legs model
    #define kUpper  1                       // This stores the ID for the torso model
    #define kHead   2                       // This stores the ID for the head model
    #define kWeapon 3                       // This stores the ID for the weapon model

    // All the animations in order from the config file (.cfg)
    enum MD3ANIMATIONS {
      // If one model is set to one of the BOTH_* animations, the other one should be too,
      // otherwise it looks really bad and confusing.

      BOTH_DEATH1 = 0,                // The first twirling death animation
      BOTH_DEAD1,                             // The end of the first twirling death animation
      BOTH_DEATH2,                    // The second twirling death animation
      BOTH_DEAD2,                             // The end of the second twirling death animation
      BOTH_DEATH3,                    // The back flip death animation
      BOTH_DEAD3,                             // The end of the back flip death animation

      // The next block is the animations that the upper body performs

      TORSO_GESTURE,                  // The torso's gesturing animation

      TORSO_ATTACK,                   // The torso's attack1 animation
      TORSO_ATTACK2,                  // The torso's attack2 animation

      TORSO_DROP,                             // The torso's weapon drop animation
      TORSO_RAISE,                    // The torso's weapon pickup animation

      TORSO_STAND,                    // The torso's idle stand animation
      TORSO_STAND2,                   // The torso's idle stand2 animation

      // The final block is the animations that the legs perform

      LEGS_WALKCR,                    // The legs's crouching walk animation
      LEGS_WALK,                              // The legs's walk animation
      LEGS_RUN,                               // The legs's run animation
      LEGS_BACK,                              // The legs's running backwards animation
      LEGS_SWIM,                              // The legs's swimming animation

      LEGS_JUMP,                              // The legs's jumping animation
      LEGS_LAND,                              // The legs's landing animation

      LEGS_JUMPB,                             // The legs's jumping back animation
      LEGS_LANDB,                             // The legs's landing back animation

      LEGS_IDLE,                              // The legs's idle stand animation
      LEGS_IDLECR,                    // The legs's idle crouching animation

      LEGS_TURN,                              // The legs's turn animation

      MAX_ANIMATIONS                  // The define for the maximum amount of animations
    };


    // This holds the header information that is read in at the beginning of the file
    struct cMd3Header
    {
      char    fileID[4];                                      // This stores the file ID - Must be "IDP3"
      int             version;                                        // This stores the file version - Must be 15
      char    strFile[68];                            // This stores the name of the file
      int             numFrames;                                      // This stores the number of animation frames
      int             numTags;                                        // This stores the tag count
      int             numMeshes;                                      // This stores the number of sub-objects in the mesh
      int             numMaxSkins;                            // This stores the number of skins for the mesh
      int             headerSize;                                     // This stores the mesh header size
      int             tagStart;                                       // This stores the offset into the file for tags
      int             tagEnd;                                         // This stores the end offset into the file for tags
      int             fileSize;                                       // This stores the file size
    };


    // This structure is used to read in the mesh data for the .md3 models
    struct cMd3MeshInfo
    {
      char    meshID[4];                                      // This stores the mesh ID (We don't care)
      char    strName[68];                            // This stores the mesh name (We do care)
      int             numMeshFrames;                          // This stores the mesh aniamtion frame count
      int             numSkins;                                       // This stores the mesh skin count
      int     numVertices;                            // This stores the mesh vertex count
      int             numTriangles;                           // This stores the mesh face count
      int             triStart;                                       // This stores the starting offset for the triangles
      int             headerSize;                                     // This stores the header size for the mesh
      int     uvStart;                                        // This stores the starting offset for the UV coordinates
      int             vertexStart;                            // This stores the starting offset for the vertex indices
      int             meshSize;                                       // This stores the total mesh size
    };


    // This is our tag structure for the .MD3 file format.  These are used link other
    // models to and the rotate and transate the child models of that model.
    struct cMd3Tag
    {
      char strName[64];                // This stores the name of the tag (I.E. "tag_torso")
      spitfire::math::cVec3 vPosition; // This stores the translation that should be performed
      float rotation[3][3];            // This stores the 3x3 rotation matrix for this frame
    };

    // This stores the bone information (useless as far as I can see...)
    struct cMd3Bone
    {
      float   mins[3];                                        // This is the min (x, y, z) value for the bone
      float   maxs[3];                                        // This is the max (x, y, z) value for the bone
      float   position[3];                            // This supposedly stores the bone position???
      float   scale;                                          // This stores the scale of the bone
      char    creator[16];                            // The modeler used to create the model (I.E. "3DS Max")
    };

    // This stores the normals and vertex indices
    struct cMd3Triangle
    {
      signed short  vertex[3];                             // The vertex for this face (scale down by 64.0f)
      unsigned char normal[2];                             // This stores some crazy normal values (not sure...)
    };

    // This stores the indices into the vertex and texture coordinate arrays
    struct cMd3Face
    {
      int vertexIndices[3];
    };


    // This stores UV coordinates
    struct cMd3TexCoord
    {
      float textureCoord[2];
    };


    // This stores a skin name (We don't use this, just the name of the model to get the texture)
    struct cMd3Skin
    {
      char strName[68];
    };



    // This class handles all of the main loading code

    class CLoadMD3
    {
    public:
      // This inits the data members
      CLoadMD3();

      // This is the function that you call to load the MD3 model
      bool ImportMD3(cModel* pModel, const char* strFileName);

      // This loads a model's .skin file
      bool LoadSkin(cModel* pModel, const char* strSkin);

      // This loads a weapon's .shader file
      bool LoadShader(cModel* pModel, const char* strShader);

    private:
      // This reads in the data from the MD3 file and stores it in the member variables,
      // later to be converted to our cool structures so we don't depend on Quake3 stuff.
      void ReadMD3Data(cModel* pModel);

      // This converts the member variables to our pModel structure, and takes the model
      // to be loaded and the mesh header to get the mesh info.
      void ConvertDataStructures(cModel* pModel, cMd3MeshInfo meshHeader);

      // This frees memory and closes the file
      void CleanUp();

      // Member Variables

      // The file pointer
      FILE* m_FilePointer;

      cMd3Header m_Header;                       // The header data

      cMd3Skin* m_pSkins;                      // The skin name data (not used)
      cMd3TexCoord* m_pTexCoords;          // The texture coordinates
      cMd3Face* m_pTriangles;          // Face/Triangle data
      cMd3Triangle* m_pVertices;           // Vertex/UV indices
      cMd3Bone* m_pBones;                      // This stores the bone data (not used)
    };


    class cMd3
    {
    public:
      int Load(const char* filename);

      bool IsValid() const { return bIsValid; }

      // This loads the model from a path and name prefix.   It takes the path and
      // model name prefix to be added to _upper.md3, _lower.md3 or _head.md3.
      bool LoadModel(const char* strPath, const char* strModelHead, const char* strModelUpper, const char* strModelLower, const char* strSkinHead, const char* strSkinUpper, const char* strSkinLower);

      // This loads the weapon and takes the same path and model name to be added to .md3
      bool LoadWeapon(const char* strPath, const char* strModel);

      // This links a model to another model (pLink) so that it's the parent of that child.
      // The strTagName is the tag, or joint, that they will be linked at (I.E. "tag_torso").
      void LinkModel(cModel* pModel, cModel* pLink, const char* strTagName);

      std::string GetHeadAnimation() const;
      std::string GetTorsoAnimation() const;
      std::string GetLegsAnimation() const;
      std::string GetWeaponAnimation() const;

      // This takes a string of an animation and sets the torso animation accordingly
      void SetTorsoAnimation(const char* strAnimation);

      // This takes a string of an animation and sets the legs animation accordingly
      void SetLegsAnimation(const char* strAnimation);


      void Update(sampletime_t currentTime);

      void Render(sampletime_t currentTime);

      // This returns a pointer to a .md3 model in the character (kLower, kUpper, kHead, kWeapon)
      cModel* GetModel(int whichPart);


    private:
      // This loads the models textures with a given path
      void LoadModelTextures(cModel* pModel, const char* strPath);

      // This loads the animation config file (.cfg) for the character
      bool LoadAnimations(const char* strConfigFile);

      // This updates the models current frame of animation, and calls SetCurrentTime()
      void UpdateModel(cModel* pModel);

      // This sets the lastTime, t, and the currentFrame of the models animation when needed
      void SetCurrentTime(cModel* pModel);

      // This recursively draws the character models, starting with the lower.md3 model
      void DrawLink(const cModel* pModel);

      // This a md3 model to the screen (not the whole character)
      void RenderModel(const cModel* pModel);

      // Member Variables

      bool bIsValid;

      // This stores the texture array for each of the textures assigned to this model
      uint32_t m_Textures[MAX_TEXTURES];

      // This stores a list of all the names of the textures that have been loaded.
      // This was created so that we could check to see if a texture that is assigned
      // to a mesh has already been loaded.  If so, then we don't need to load it again
      // and we can assign the textureID to the same textureID as the first loaded texture.
      // You could get rid of this variable by doing something tricky in the texture loading
      // function, but I didn't want to make it too confusing to load the textures.
      std::vector<std::string> strTextures;

      // These are are models for the character's head and upper and lower body parts
      cModel m_Head;
      cModel m_Upper;
      cModel m_Lower;

      // This stores the players weapon model (optional load)
      cModel m_Weapon;
    };












    ///////////////////////////////////////////////////////////////////////////////////
    //
    // This version of the tutorial incorporates the animation data stored in the MD3
    // character files.  We will be reading in the .cfg file that stores the animation
    // data.  The rotations and translations of the models will be done using a matrix.
    // There will be no more calls to glTranslatef().  To create the rotation and
    // translation matrix, quaternions will be used.  This is because quaternions
    // are excellent for interpolating between 2 rotations, as well as not overriding
    // another translation causing "gimbal lock".
    //
    // So, why do we need to interpolate?  Well, the animations for the character are
    // stored in key frames.  Instead of saving each frame of an animation, key frames
    // are stored to cut down on memory and disk space.  The files would be huge if every
    // frame was saved for every animation, as well as creating a huge memory footprint.
    // Can you imagine having 10+ models in memory with all of that animation data?
    //
    // The animation key frames are stored in 2 ways.  The torso and legs mesh have vertices
    // stored for each of the key frames, along with separate rotations and translations
    // for the basic bone animation.  Remember, that each .md3 represents a bone, that needs
    // to be connected at a joint.  For instance, the torso is connected to the legs, and the
    // head is connected to the torso.  So, that makes 3 bones and 2 joints.  If you add the
    // weapon, the weapon is connected to the hand joint, which gives us 4 bones and 3 joints.
    // Unlike conventional skeletal animation systems, the main animations of the character's
    // movement, such as a gesture or swimming animation, are done not with bones, but with
    // vertex key frames, like in the .md2 format. Since the lower, upper, head and weapon models
    // are totally different models, which aren't seamlessly connected to each other, then parent
    // node needs to end a message (a translation and rotation) down to all it's child nodes to
    // tell them where they need to be in order for the animation to look right.  A good example
    // of this is when the legs has the DEATH3 animation set,  The legs might kick back into a back
    // flip that lands the character on their face, dead.  Well, since the main models are separate,
    // if the legs didn't tell the torso where to go, then the model's torso would stay in the same
    // place and the body would detach itself from the legs.  The exporter calculates all this stuff
    // for you of course.
    //
    // But getting back to the interpolation, since we use key frames, if we didn't interpolate
    // between them, the animation would look very jumping and unnatural.  It would also go too
    // fast.  By interpolating, we create a smooth transition between each key frame.
    //
    // As seen in the .md2 tutorials, interpolating between vertices is easy if we use the
    // linear interpolation function:  p(t) = p0 + t(p1 - p0).  The same goes for translations,
    // since it's just 2 3D points.  This is not so for the rotations.  The Quake3 character
    // stores the rotations for each key frame in a 3x3 matrix.  This isn't a simple linear
    // interpolation that needs to be performed.  If we convert the matrices to a quaternion,
    // then use spherical linear interpolation (SLERP) between the current frame's quaternion
    // and the next key frame's quaternion, we will have a new interpolated quaternion that
    // can be converted into a 4x4 matrix to be applied to the current model view matrix in OpenGL.
    // After finding the interpolated translation to be applied, we can slip that into the rotation
    // matrix before it's applied to the current matrix, which will require only one matrix command.
    //
    // You'll notice that in the CreateFromMatrix() function in our quaternion class, I allow a
    // row and column count to be passed in.  This is just a dirty way to allow a 3x3 or 4x4 matrix
    // to be passed in.  Instead of creating a whole new function and copy and pasting the main
    // code, it seemed fitting for a tutorial.  It's obvious that the quaternion class is missing
    // a tremendous amount of functions, but I chose to only keep the functions that we would use.
    //
    // For those of you who don't know what interpolation us, here is a section abstracted
    // from the MD2 Animation tutorial:
    //
    // -------------------------------------------------------------------------------------
    // Interpolation: Gamedev.net's Game Dictionary say interpolation is "using a ratio
    // to step gradually a variable from one value to another."  In our case, this
    // means that we gradually move our vertices from one key frame to another key frame.
    // There are many types of interpolation, but we are just going to use linear.
    // The equation for linear interpolation is this:
    //
    //                              p(t) = p0 + t(p1 - p0)
    //
    //                              t - The current time with 0 being the start and 1 being the end
    //                              p(t) - The result of the equation with time t
    //                              p0 - The starting position
    //                              p1 - The ending position
    //
    // Let's throw in an example with numbers to test this equation.  If we have
    // a vertex stored at 0 along the X axis and we wanted to move the point to
    // 10 with 5 steps, see if you can fill in the equation without a time just yet.
    //
    // Finished?  You should have come up with:
    //
    //                              p(t) = 0 + t(10 - 0)
    //                              p(t) = 0 + 10t
    //                              p(t) = 10t
    //
    // Now, all we need it a time from 0 to 1 to pass in, which will allow us to find any
    // point from 0 to 10, depending on the time.  Since we wanted to find out the distance
    // we need to travel each frame if we want to reach the end point in 5 steps, we just
    // divide 1 by 5: 1/5 = 0.2
    //
    // We can then pass this into our equation:
    //
    //                              p(0.2) = 10 * 0.2
    //                              p(0.2) = 2
    //
    // What does that tell us?  It tells us we need to move the vertex along the x
    // axis each frame by a distance of 2 to reach 10 in 5 steps.  Yah yah, this isn't
    // rocket science, but it's important to know that what your mind would have done
    // immediately without thinking about it, is linear interpolation.
    //
    // Are you starting to see how this applies to our model?  If we only read in key
    // frames, then we need to interpolate every vertex between the current and next
    // key frame for animation.  To get a perfect idea of what is going on, try
    // taking out the interpolation and just render the key frames.  You will notice
    // that you can still see what is kinda going on, but it moves at an incredible pace!
    // There is not smoothness, just a really fast jumpy animation.
    // ------------------------------------------------------------------------------------


    /////////////////////////////////////////////////////////////////////////////////
    //
    // There were a bunch of functions added to this file, since the last MD3 tutorial.
    // We added an enum for the animation indices, along with some defines for GetModel().
    //
    // The CLoadMD3 class stayed the same, but these functions were added to our CLoadMD3 class:
    //
    //      This takes a string of an animation and sets the torso animation accordingly
    //      void SetTorsoAnimation(const char* strAnimation);
    //
    //      This takes a string of an animation and sets the legs animation accordingly
    //      void SetLegsAnimation(const char* strAnimation);
    //
    //      This returns a pointer to a .md3 model in the character (kLower, kUpper, kHead, kWeapon)
    //      cModel* GetModel(int whichPart);
    //
    //      This loads the animation config file (.cfg) for the character
    //      bool LoadAnimations(const char* strConfigFile);
    //
    //      This updates the models current frame of animation, and calls SetCurrentTime()
    //      void UpdateModel(cModel* pModel);
    //
    //      This sets the lastTime, t, and the currentFrame of the models animation when needed
    //      void SetCurrentTime(cModel* pModel);
    //

    // Below I will sum up everything that we went over in this tutorial.  I don't
    // think it was a ton of things to sift through, but certainly model loading and
    // animation are huge subjects that need a lot of code.  You can't just call
    // glLoadModel() and glAnimateModel() for this stuff :)
    //
    // First of all, we added a basic quaternion class to our tutorial.  This is used
    // to take a matrix, convert it to a quaternion, interpolate between another
    // quaternion that was converted to a matrix, then turned back into a matrix.
    // This is because quaternions are a great way to interpolate between rotations.
    // If you wanted to use the glRotatef() and glTranslatef() functions, you would
    // want to convert the interpolated quaternion to an axis angle representation,
    // which might be less code, but not as useful in the long run.
    //
    // The next important thing that was added was the interpolation between vertex
    // key frames.  We learned earlier (top of Md3.cpp) that not most of the animation
    // is done through key frame vertices, not bones.  The only bone animation that is
    // done is with the joints that connect the .md3 models together.  Half Life, for
    // example, uses full on skeletal animation.
    //
    // Also, in this tutorial we added the code to parse the animation config file (.cfg).
    // this stores the animation information for each animation.  The order of the are
    // important.  For the most part, the config files are the same format.  As discussed
    // in the previous MD3 tutorial, there is a few extra things in the config file that
    // we don't read in here, such as footstep sounds and initial positions.  The tutorial
    // was not designed to be a robust reusable Quake3 character loader, but to teach you
    // how it works for the most part.  Other things are extra credit :)
    //
    // Another important thing was our timing system.  Since we were dealing with multiple
    // models that had different frames per second, we needed to add some variables to our
    // cModel class to hold some things like elapsedTime and the current t value.  This
    // way, no static variables had to be created, like in the .MD2 tutorial.
  }
}

#endif

#ifndef CMODEL_ANIMATION_H
#define CMODEL_ANIMATION_H

namespace breathe
{
  namespace character
  {
    // A face in this case is a triangle, perhaps we could rename this to tTriangle instead?
    struct cFace
    {
      int vertIndex[3];  // indicies for the verts that make up this triangle
      int coordIndex[3]; // indicies for the tex coords to texture this face
    };

    struct cMaterial
    {
      char  strName[255]; // The texture name
      char  strFile[255]; // The texture file name (If this is set it's a texture map)
      int   texureId;     // the texture ID
      float uTile;        // u tiling of texture
      float vTile;        // v tiling of texture
      float uOffset;      // u offset of texture
      float vOffset;      // v offset of texture
    };

    // This holds all the information for our model/scene.
    class cStaticModel
    {
    public:
      cStaticModel() : numOfVerts(0), numOfFaces(0), numTexVertex(0), materialID(0), bHasTexture(false), pVerts(nullptr), pNormals(nullptr), pTexVerts(nullptr), pFaces(nullptr) { strName[0] = 0; }

      int  numOfVerts;      // The number of verts in the model
      int  numOfFaces;      // The number of faces in the model
      int  numTexVertex;    // The number of texture coordinates
      int  materialID;      // The texture ID to use, which is the index into our texture array
      bool bHasTexture;     // This is TRUE if there is a texture map for this object
      char strName[255];    // The name of the object
      spitfire::math::cVec3* pVerts;    // The object's vertices
      spitfire::math::cVec3* pNormals;  // The object's normals
      spitfire::math::cVec2* pTexVerts; // The texture's UV coordinates
      cFace* pFaces;        // The faces information of the object
    };

    // This holds our information for each animation of the Quake model.
    // A STL vector list of this structure is created in our cModel structure below.
    class cAnimationSet
    {
    public:
      cAnimationSet() : startFrame(0), endFrame(0), loopingFrames(0), framesPerSecond(0) { strName[0] = 0; }

      char strName[255];   // This stores the name of the animation (I.E. "TORSO_STAND")
      int startFrame;      // This stores the first frame number for this animation
      int endFrame;        // This stores the last frame number for this animation
      int loopingFrames;   // This stores the looping frames for this animation (not used)
      int framesPerSecond; // This stores the frames per second that this animation runs
    };

    struct cModelTag
    {
      std::string strName;             // This stores the name of the tag (I.E. "tag_torso")
      spitfire::math::cVec3 vPosition; // This stores the translation that should be performed
      float rotation[3][3];            // This stores the 3x3 rotation matrix for this frame
    };

    // This our model structure
    class cModel
    {
    public:
      cModel() :
        currentAnim(0),
        currentFrame(0),
        nextFrame(0),
        t(0.0f),
        lastTime(0.0f),

        numOfTags(0),
        pLinks(nullptr),
        pTags(nullptr)
      {}

      ~cModel() { Destroy(); }

      void Destroy()
      {
        Clear();
      }

      std::vector<cMaterial> materials;  // The list of material information (Textures and colors)
      std::vector<cStaticModel> objects;      // The object list for our model

      int currentAnim;          // The current index into pAnimations list
      int currentFrame;          // The current frame of the current animation
      int nextFrame;            // The next frame of animation to interpolate too
      float t;              // The ratio of 0.0f to 1.0f between each key frame
      float lastTime;            // This stores the last time that was stored
      std::vector<cAnimationSet> animations; // The list of animations

      // NOTE: We shouldn't be using raw pointers
      int numOfTags;            // This stores the number of tags in the model
      cModel** pLinks;    // This stores a list of pointers that are linked to this model
      cModelTag* pTags;      // This stores all the tags for the model animations

      void Clear()
      {
        // Go through all the objects in the model
        const size_t n = objects.size();
        for(size_t i = 0; i < n; i++) {
          // Free the faces, normals, vertices, and texture coordinates.
          if (objects[i].pFaces != nullptr)    delete [] objects[i].pFaces;
          if (objects[i].pNormals != nullptr)  delete [] objects[i].pNormals;
          if (objects[i].pVerts != nullptr)    delete [] objects[i].pVerts;
          if (objects[i].pTexVerts != nullptr) delete [] objects[i].pTexVerts;
        }

        // Free the tags associated with this model
        if (pTags != nullptr) delete [] pTags;

        // Free the links associated with this model (We use free because we used malloc())
        if (pLinks != nullptr) free(pLinks);

        materials.clear();
        objects.clear();
        animations.clear();

        pLinks = nullptr;
        pTags = nullptr;

        currentAnim = 0;
        currentFrame = 0;
        nextFrame = 0;
        t = 0.0f;
        lastTime = 0.0f;
        numOfTags = 0;
      }
    };
  }

  namespace render
  {
    namespace model
    {
      class cAnimation : public cModel
      {
      public:
        cAnimation();
        ~cAnimation();

        int Load(const string_t& sFilename);
        void Update(sampletime_t currentTime);

        //std::vector<cStatic*> vFrame; // A vector of all the statics in this model
      };
    }
  }
}

#endif // CMODEL_ANIMATION_H

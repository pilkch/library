#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include <breathe/breathe.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>

#include <breathe/render/cContext.h>
#include <breathe/render/camera.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/model/cHeightmap.h>

// TODO: We have to remove this, there has to be a better way of doing this?
#include <breathe/render/model/cMd3.h>



// GeodeNode - leaf node from which Renderable leaves are hung - no child nodes, only child Renderables
// BillboardNode
// ParticleSystemNode
// Renderables
// StateSets

// Possible Future Work:
// ImposterNode - adds support for hierarchical image chaching
// OccluderNode

// Never:
// SequenceNode - Automatically steps through children

// GroupNode at the top containing the whole graph
// GroupNodes, LODNodes, TransformNodes, SwitchNodes in the middle
// GeodeNodes/BillboardNodes are the leaf nodes which contain...
// Renderables which are leaves that contain the geometry and can be drawn.
// StateSets attached to Nodes and Renderables, state inherits from parents too.

// http://www.openscenegraph.org/documentation/VRLabLecture/HowToLearnAbout/Nodes.html
// http://www.openscenegraph.org/projects/osg/browser/OpenSceneGraph/trunk/src/osgUtil/Optimizer.cpp?format=txt

// cSceneNode
//     |
//     v
// cSwitchNode, cSequenceNode list<cSceneNodeRef> frame, cRenderableNode


// cSceneNode pure virtual
//     |
//     v
// cRenderableNode pure virtual
//     |
//     v
// cRenderableParticleEffect


// cRenderableNode
//     |
//     v
// cRenderableGroupNode
// {
//   std::list<cRenderableNodeRef> children
// }

namespace breathe
{
  class cCamera;

  namespace sky
  {
    class cSkySystem;
    class cSkyDomeAtmosphereRenderer;
  }

  typedef cSmartPtr<sky::cSkySystem> cSkySystemRef;

  namespace scenegraph_common
  {
    // Every node must have a unique name, this is for generating a name when none has been specified.
    string_t SceneNodeGenerateUniqueName();


    // State classes for keeping track of what state we are in.
    // Default compiler generated copy constructors are fine for these simple classes.

    class cStateBoolean
    {
    public:
      cStateBoolean() : bHasValidValue(false), bTurnedOn(false) {}

      bool IsValidAndTurnedOn() const { return (bHasValidValue && bTurnedOn); }

      void Clear() { bHasValidValue = false; bTurnedOn = false; }

      bool operator==(const cStateBoolean& rhs) const;
      bool operator!=(const cStateBoolean& rhs) const { return !(*this == rhs); }

      bool bHasValidValue;
      bool bTurnedOn;
    };

    inline bool cStateBoolean::operator==(const cStateBoolean& rhs) const
    {
      return (bHasValidValue == rhs.bHasValidValue) && (bTurnedOn == rhs.bTurnedOn);
    }


    class cStateBooleanWithFloat
    {
    public:
      cStateBooleanWithFloat() : bHasValidValue(false), bTurnedOn(false), fValue(0.0f) {}

      bool IsValidAndTurnedOn() const { return (bHasValidValue && bTurnedOn); }

      void Clear() { bHasValidValue = false; bTurnedOn = false; fValue = 0.0f; }

      bool operator==(const cStateBooleanWithFloat& rhs) const;
      bool operator!=(const cStateBooleanWithFloat& rhs) const { return !(*this == rhs); }

      bool bHasValidValue;
      bool bTurnedOn;
      float fValue;
    };

    inline bool cStateBooleanWithFloat::operator==(const cStateBooleanWithFloat& rhs) const
    {
      return (bHasValidValue == rhs.bHasValidValue) && (bTurnedOn == rhs.bTurnedOn) && (math::IsApproximatelyEqual(fValue, rhs.fValue));
    }


    class cStateTexture
    {
    public:
      cStateTexture() : bHasValidValue(false), bTurnedOn(false) {}

      bool IsValidAndTurnedOn() const { return (bHasValidValue && bTurnedOn); }

      void Clear() { bHasValidValue = false; bTurnedOn = false; pTexture.reset(); }

      bool operator==(const cStateTexture& rhs) const;
      bool operator!=(const cStateTexture& rhs) const { return !(*this == rhs); }

      bool bHasValidValue;
      bool bTurnedOn;
      render::cTextureRef pTexture;
    };

    inline bool cStateTexture::operator==(const cStateTexture& rhs) const
    {
      return (bHasValidValue == rhs.bHasValidValue) && (bTurnedOn == rhs.bTurnedOn) && (pTexture == rhs.pTexture);
    }


    class cStateColour
    {
    public:
      cStateColour() : bHasValidValue(false), bTurnedOn(false) {}

      bool IsValidAndTurnedOn() const { return (bHasValidValue && bTurnedOn); }

      void Clear() { bHasValidValue = false; bTurnedOn = false; colour.Clear(); }

      bool operator==(const cStateColour& rhs) const;
      bool operator!=(const cStateColour& rhs) const { return !(*this == rhs); }

      bool bHasValidValue;
      bool bTurnedOn;
      math::cColour colour;
    };

    inline bool cStateColour::operator==(const cStateColour& rhs) const
    {
      return (bHasValidValue == rhs.bHasValidValue) && (bTurnedOn == rhs.bTurnedOn) && (colour == rhs.colour);
    }


    class cStateShader
    {
    public:
      cStateShader() : bHasValidValue(false), bTurnedOn(false) {}

      bool IsValidAndTurnedOn() const { return (bHasValidValue && bTurnedOn); }

      void Clear() { bHasValidValue = false; bTurnedOn = false; pShader.reset(); }

      bool operator==(const cStateShader& rhs) const;
      bool operator!=(const cStateShader& rhs) const { return !(*this == rhs); }

      bool bHasValidValue;
      bool bTurnedOn;
      render::cShaderRef pShader;
    };

    inline bool cStateShader::operator==(const cStateShader& rhs) const
    {
      return (bHasValidValue == rhs.bHasValidValue) && (bTurnedOn == rhs.bTurnedOn) && (pShader == rhs.pShader);
    }


    class cStateVertexBufferObject
    {
    public:
      cStateVertexBufferObject() : bHasValidValue(false), bTurnedOn(false) {}

      bool IsValidAndTurnedOn() const { return (bHasValidValue && bTurnedOn); }

      void Clear() { bHasValidValue = false; bTurnedOn = false; pVertexBufferObject.reset(); }

      bool HasValidValue() const { return bHasValidValue; }
      void SetHasValidValue(bool _bHasValidValue) { bHasValidValue = _bHasValidValue; }

      bool IsEnabled() const { return bTurnedOn; }
      void SetEnabled(bool bEnable) { bTurnedOn = bEnable; }

      render::cVertexBufferObjectRef GetVertexBufferObject() const { return pVertexBufferObject; }
      void SetVertexBufferObject(render::cVertexBufferObjectRef _pVertexBufferObject) { pVertexBufferObject = _pVertexBufferObject; }

      bool operator==(const cStateVertexBufferObject& rhs) const;
      bool operator!=(const cStateVertexBufferObject& rhs) const { return !(*this == rhs); }

    private:
      bool bHasValidValue;
      bool bTurnedOn;
      render::cVertexBufferObjectRef pVertexBufferObject;
    };

    inline bool cStateVertexBufferObject::operator==(const cStateVertexBufferObject& rhs) const
    {
      return (bHasValidValue == rhs.bHasValidValue) && (bTurnedOn == rhs.bTurnedOn) && (pVertexBufferObject == rhs.pVertexBufferObject);
    }


    enum class GEOMETRY_TYPE {
      POINTS,
      LINES,
      TRIANGLES,
      QUADS,

      // Helper states
      DEFAULT = TRIANGLES
    };
  }

  namespace scenegraph3d
  {
    class cSceneGraph;
    class cUpdateVisitor;
    class cCullVisitor;



    class cObject
    {
    public:
      virtual ~cObject() {}
    };

    typedef cObject* cObjectRef;


    class cEntity : public cObject
    {
    public:

    };

    typedef cEntity* cEntityRef;

    class cParticleSystem : public cObject
    {
    public:

    };

    typedef cParticleSystem* cParticleSystemRef;

    class cLight : public cObject
    {
    public:

    };

    typedef cLight* cLightRef;


    /*class cSceneGraphModel : public cObject
    {
    public:
      render::model::cStaticRef pModel;

      void Update(sampletime_t currentTime)
      {

      }
    };

    class cSceneGraphCubemap
    {
    public:
      math::cVec3 v3Position;

      std::string sFilename;
    };

    class cSceneGraphSpawn
    {
    public:
      cSceneGraphSpawn();

      math::cVec3 v3Position;
      math::cVec3 v3Rotation;
    };


    const unsigned int NODE_INACTIVE=0;
    const unsigned int NODE_UNLOAD=1;
    const unsigned int NODE_ACTIVE=20; //Seconds since the player has been in the vicinity

    class cSceneNode : public cRenderable
    {
    public:
      cSceneNode(cSceneGraph* p, std::string sNewFilename);
      ~cSceneNode();

      void Load();
      void Unload();


      unsigned int uiStatus;

      float fFogDistance;

      math::cColour colourFog;

      std::string sFilename;
      std::string sName;
      std::string sCRC;

      std::vector<cSceneGraphModelRef>vModel;

    private:
      cSceneGraph* pLevel;
    };*/


    class cStateSet
    {
    public:
      friend class cRenderGraph;
      friend class cRenderVisitor;

      enum class PRIORITY {
        FIRST = -3,
        SECOND = -2,
        THIRD = -1,
        OPAQUE = 0,
        DIFFUSE = 0,
        NORMAL = 0,
        TRANSPARENT,
        LAST
      };

      cStateSet();
      cStateSet(const cStateSet& rhs);

      cStateSet& operator=(const cStateSet& rhs);

      bool operator==(const cStateSet& rhs) const;
      bool operator!=(const cStateSet& rhs) const { return !(*this == rhs); }

      scenegraph_common::cStateVertexBufferObject& GetVertexBufferObject() { return vertexBufferObject; }
      const scenegraph_common::cStateVertexBufferObject& GetVertexBufferObject() const { return vertexBufferObject; }

      // TEMPORARY*******************************************************************************************
      void SetStateFromMaterial(render::material::cMaterialRef pMaterial);

      void SetGeometryTypeTriangles() { geometryType = scenegraph_common::GEOMETRY_TYPE::TRIANGLES; }
      void SetGeometryTypeQuads() { geometryType = scenegraph_common::GEOMETRY_TYPE::QUADS; }

    private:
      void Assign(const cStateSet& rhs);
      void Clear();

      PRIORITY priority;

      scenegraph_common::cStateBoolean alphablending;
      scenegraph_common::cStateTexture texture[render::MAX_TEXTURE_UNITS];
      scenegraph_common::cStateShader shader;
      scenegraph_common::cStateVertexBufferObject vertexBufferObject;
      scenegraph_common::GEOMETRY_TYPE geometryType;

      // in order of sorting
      // bool bUseIsShadowCasting;
      // bool bIsShadowCasting;
      // bool bUseRenderPriority;
    };

    inline cStateSet::cStateSet()
    {
      Clear();
    }

    inline cStateSet::cStateSet(const cStateSet& rhs)
    {
      Assign(rhs);
    }

    inline cStateSet& cStateSet::operator=(const cStateSet& rhs)
    {
      Assign(rhs);
      return *this;
    }

    inline void cStateSet::Assign(const cStateSet& rhs)
    {
      ASSERT(render::MAX_TEXTURE_UNITS == 3);

      priority = rhs.priority;
      alphablending = rhs.alphablending;
      texture[0] = rhs.texture[0];
      texture[1] = rhs.texture[1];
      texture[2] = rhs.texture[2];
      shader = rhs.shader;
      vertexBufferObject = rhs.vertexBufferObject;
      geometryType = rhs.geometryType;
    }

    inline bool cStateSet::operator==(const cStateSet& rhs) const
    {
      ASSERT(render::MAX_TEXTURE_UNITS == 3);

      return (
        (priority == rhs.priority) && (alphablending == rhs.alphablending) &&
        (texture[0] == rhs.texture[0]) && (texture[1] == rhs.texture[1]) && (texture[2] == rhs.texture[2]) &&
        (shader == rhs.shader) && (vertexBufferObject == rhs.vertexBufferObject) && (geometryType == rhs.geometryType)
      );
    }

    class cSceneNode;

    typedef cSmartPtr<cSceneNode> cSceneNodeRef;

    // Uses a quaternion to work out world rotation
    // Only call GenerateBoundingVolume at the very end of setting rotations and positions for all nodes

    class cSceneNode : public boost::enable_shared_from_this<cSceneNode>
    {
    public:
      friend class cUpdateVisitor;
      friend class cCullVisitor;

      // Child nodes
      typedef std::list<cSceneNodeRef>::iterator child_iterator;
      typedef std::list<cSceneNodeRef>::const_iterator child_const_iterator;

      // Objects
      typedef std::map<std::string, cObjectRef>::iterator object_iterator;
      typedef std::map<std::string, cObjectRef>::const_iterator const_object_iterator;


      virtual ~cSceneNode();

      const string_t& GetUniqueName() const { return sUniqueName; }
      void SetUniqueName(const string_t& _sUniqueName) { sUniqueName = _sUniqueName; }

#ifndef NDEBUG
      bool IsWithinNode(cSceneNodeRef pNode) const; // Determines whether this node is in the scene graph, ie. whether it's ultimate ancestor is the root scene node.

      // Checks that pChild is one of our direct children
      bool IsParentOfChild(const cSceneNodeRef pChild) const;
#endif

      cSceneNodeRef GetParent() const { return pParent; }

      void AttachChild(cSceneNodeRef pChild); // Calls the virtual _AttachChild so that a scenenode can choose how to add it or even not add it at all
      void DetachChildForUseLater(cSceneNodeRef pChild); // Doesn't touch children of pChild, just detaches from the scenenode, so you can still reference pChild and reinsert it later
      void DeleteChildRecursively(cSceneNodeRef pChild); // Removes pChild from the scenenode and also calls DeleteAllChildrenRecursively on each of the children of pChild
      void DeleteAllChildrenRecursively() { _DeleteAllChildrenRecursively(); }


      void AttachObject(cObjectRef pObject); // Calls the virtual _AttachObject so that a scenenode can choose how to add it or even not add it at all
      void DetachObjectForUseLater(cObjectRef pObject); // Detaches the object from the scenenode, so you can still reference pObject and reinsert it later


      bool IsEnabled() const { return bIsEnabled; }
      void SetEnabled(bool _bIsEnabled) { bIsEnabled = _bIsEnabled; }

      bool IsDirty() const { return bIsDirty; }
      void SetDirty();

      void SetVisible(bool bVisible);
      void SetPosition(const math::cVec3& position) { SetRelativePosition(position); }
      void SetRotation(const math::cQuaternion& rotation) { SetRelativeRotation(rotation); }

      const math::cVec3& GetRelativePosition() const { return relativePosition; }
      const math::cQuaternion& GetRelativeRotation() const { return relativeRotation; }
      void SetRelativePosition(const math::cVec3& position);
      void SetRelativeRotation(const math::cQuaternion& rotation);

      math::cVec3 GetAbsolutePosition() const; // Calculated based on the parents of this node
      math::cQuaternion GetAbsoluteRotation() const; // Calculated based on the parents of this node

      void SetScale(const math::cVec3& scale) { relativeScale = scale; }
      const math::cVec3& GetScale() const { return relativeScale; }


      const math::cSphere& GetBoundingSphere() const { return boundingSphere; }
      const math::cBox& GetBoundingBox() const { return boundingBox; }

      // We call this from cSceneGraph::Update
      void UpdateBoundingVolumeAndSetNotDirty();

#ifdef BUILD_DEBUG
      bool IsShowingBoundingBox() const;
      void SetShowingBoundingBox(bool bShow) { bIsShowingBoundingBox = bShow; }
#endif

      void Update(cUpdateVisitor& visitor) { _Update(visitor); }
      void Cull(cCullVisitor& visitor) { _Cull(visitor); }

      cStateSet& GetStateSet() { return stateset; }
      const cStateSet& GetStateSet() const { return stateset; }

    protected:
      // Only the derived classes can call use this constructor
      cSceneNode();

      math::cMat4 GetAbsoluteMatrix() const;

      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);

      float_t UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius();

      // Only the derived class will know how to get the radius from our possible children
      virtual float_t _UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius() { return math::cEPSILON; }


      string_t sUniqueName; // This is only for debugging purposes

#ifdef BUILD_DEBUG
      bool bIsShowingBoundingBox;
#endif

      bool bIsVisible;
      bool bIsEnabled;

      bool bIsDirty;

      bool bUseIsDynamic;
      bool bIsDynamic;

      bool bHasRelativePosition;
      math::cVec3 relativePosition;

      bool bHasRelativeRotation;
      math::cQuaternion relativeRotation;

      bool bUseRelativeScale;
      math::cVec3 relativeScale;

      cStateSet stateset;

      math::cSphere boundingSphere;
      math::cBox boundingBox;

      cSceneNodeRef pParent; // Each node has exactly one parent, no more no less.  pRoot is the only exception, pRoot->pParent == nullptr;

    private:
      cSceneNode(const cSceneNode&); // Prevent copying
      cSceneNode& operator=(const cSceneNode&); // Prevent copying

      void GenerateBoundingVolume();

      // NOTE: If you override any of these methods you should override all of them
      virtual void _AttachChild(cSceneNodeRef pChild);
      virtual void _DetachChild(cSceneNodeRef pChild);
      virtual void _DeleteChildRecursively(cSceneNodeRef pChild);
      virtual void _DeleteAllChildrenRecursively();

      std::list<cSceneNodeRef> children;
      std::map<std::string, cObjectRef> objects;
    };

    inline cSceneNode::~cSceneNode()
    {
      DeleteAllChildrenRecursively();
    }


    class cGroupNode : public cSceneNode {};
    typedef cSmartPtr<cGroupNode> cGroupNodeRef;


    // Adds everything below this node to the list of 2D objects to be rendered, ie. HUD
    class cProjection2D : public cSceneNode
    {
    public:
      void SetChild(cSceneNodeRef _pChild) { pChild = _pChild; }

    private:
      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);

      cSceneNodeRef pChild;
    };

    class cModelNode : public cSceneNode
    {
    public:

    private:
      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);
    };

    typedef cSmartPtr<cModelNode> cModelNodeRef;

    class cAnimationNode : public cSceneNode
    {
    public:
      character::cAnimation animation;

    private:
      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);
    };

    typedef cSmartPtr<cAnimationNode> cAnimationNodeRef;

    class cParticleSystemNode : public cSceneNode
    {
    public:

    private:
      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);
    };

    typedef cSmartPtr<cParticleSystemNode> cParticleSystemNodeRef;

    class cLightNode : public cSceneNode
    {
    public:

    private:
      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);

      math::cColour colourAmbient;
      math::cColour colourDiffuse;
      float_t fMaximumDistanceRadius;
    };

    typedef cSmartPtr<cLightNode> cLightNodeRef;


    class cSwitchNode : public cSceneNode
    {
    public:
      cSwitchNode() : index(0) {}

      void SetIndex(size_t index);

    private:
      void _AttachChild(cSceneNodeRef pNode);
      void _DetachChild(cSceneNodeRef pChild);

      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);

      size_t index;
      std::vector<cSceneNodeRef> node;
    };

    inline void cSwitchNode::SetIndex(size_t _index)
    {
      // Make sure that we set our new index to a reasonable value
      const size_t n = node.size();
      ASSERT(n != 0);
      ASSERT(_index < n);
      index = _index;
    }

    class cLODNode : public cSceneNode
    {
    public:
      cLODNode() : index(0) {}

      void SetLOD(size_t LOD);

      size_t GetNumberOfChildren() const { return node.size(); }

    private:
      void _AttachChild(cSceneNodeRef pNode);
      void _DetachChild(cSceneNodeRef pChild);

      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);

      size_t index;
      std::vector<cSceneNodeRef> node;
    };

    inline void cLODNode::SetLOD(size_t LOD)
    {
      // Make sure that we set our new index to a reasonable value
      const size_t n = node.size();
      ASSERT(n != 0);
      ASSERT(LOD < n);
      index = LOD;
    }

    typedef cSmartPtr<cLODNode> cLODNodeRef;


    class cPagedLODNodeChild : public cSceneNode
    {
    public:
      void Create(size_t x, size_t y);

      void LoadAndSetTerrainLOD0();
      void LoadAndSetTerrainLOD1();
      void LoadAndSetTerrainLOD2();

      void LoadGrass();
      void UnloadGrass();

    private:
      void LoadTerrainLOD(size_t index, size_t nWidthOrHeight);

      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);

      game::cTerrainHeightMap loader;

      cLODNodeRef terrain; // Terrain heightmap
      //std::vector<cGeometryNodeRef> grass; // One cGeometryNode for each grass
      cLODNodeRef trees; // Anything else gets added here such as trees
    };

    typedef cSmartPtr<cPagedLODNodeChild> cPagedLODNodeChildRef;

    class cPagedLODNode : public cSceneNode
    {
    public:
      void SetNumberOfNodes(size_t width, size_t height);

      void Clear();

    private:
      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);

      std::vector<cPagedLODNodeChildRef> node;

      // TODO: Could try a quadtree
      // cQuadTree<cPagedLODNodeChildRef> quadtree;
    };

    typedef cSmartPtr<cPagedLODNode> cPagedLODNodeRef;



    // Octree for spatially representing the world
    class cSpatialGraphNode
    {
    public:
      cSpatialGraphNode();
      virtual ~cSpatialGraphNode() {}

      void Update(cUpdateVisitor& visitor) { _Update(visitor); }
      void Cull(cCullVisitor& visitor) { _Cull(visitor); }

    private:
      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);

      cSpatialGraphNode* pChild[8];
    };


    // 3D Spatial Nodes

    // A quadtree node maintains either zero or four children, depending upon whether it is a leaf node or a regular node.
    //class cQuadtreeNode
    //{
    //  public:
    //  // A quadtree node maintains either zero or four children, depending upon whether it is a leaf node or a regular node.
    //    bool IsValid() { return child.empty() || child.size() == 8; }
    //
    //  private:
    //    std::vector<cQuadtreeNode*> child;
    //};

    // /*typedef cQuadtreeNode cBSPNode; // Indoors - Quake 3 style*/
    //typedef cQuadtreeNode cOctreeNode; // Generic - outer space, terrain, buildings etc.
    //typedef cOctreeNode cPagedLandscapeNode; // Heightmap octree - Battlefield series


    // 2D Spatial Nodes

    //class cScreenSpaceNode
    //{
    //public:
    //  bool bHasMaterial;
    //};

    //class cRectangleNode
    //{
    //public:
    //  cVec2 position;
    //  cVec2 size;
    //};



    class cRenderGraphTransparentPair
    {
    public:
      cStateSet* pStateSet;
      spitfire::math::cMat4 matAbsolutePositionAndRotation;
    };

      // TODO: We have to remove this, there has to be a better way of doing this?
    class cRenderGraphMd3Pair
    {
    public:
      character::cMd3* pModel;
      spitfire::math::cMat4 matAbsolutePositionAndRotation;
    };

    class cRenderGraph
    {
    public:
      friend class cCullVisitor;
      friend class cRenderVisitor;

      // TODO: We have to remove this, there has to be a better way of doing this?
      void AddMD3Model(character::cMd3* pModel, const spitfire::math::cMat4& matAbsolutePositionAndRotation);

      void AddRenderable(cStateSet* pStateSet, const spitfire::math::cMat4& matAbsolutePositionAndRotation);
      void Clear();

    private:
      // TODO: We have to remove this, there has to be a better way of doing this?
      std::list<cRenderGraphMd3Pair> md3Models;

      typedef std::list<spitfire::math::cMat4> cRenderableList;

      // Opaque (states : list of renderables with that state)
      std::map<cStateSet*, cRenderableList*> mOpaque;

      // Transparent (distance from the camera : renderable at that position)
      // TODO: Should this be multimap?
      std::map<float, cRenderGraphTransparentPair> mTransparent;
    };

    inline void cRenderGraph::Clear()
    {
      mOpaque.clear();
      mTransparent.clear();
      md3Models.clear();
    }


    class cUpdateVisitor
    {
    public:
      explicit cUpdateVisitor(cSceneGraph& scenegraph);

      void Visit(cSceneNode& node) { node.Update(*this); }
      void Visit(cGroupNode& node) { node.Update(*this); }
      void Visit(cSwitchNode& node) { node.Update(*this); }
      void Visit(cLODNode& node) { node.Update(*this); }
      void Visit(cModelNode& node) { node.Update(*this); }
      void Visit(cPagedLODNode& node) { node.Update(*this); }
      void Visit(cAnimationNode& node) { node.Update(*this); }

      void Visit(sky::cSkyDomeAtmosphereRenderer& node);

    private:

    };




    //                                    cSceneNode
    //                  cLODNode                                cPagedLODNode
    //         cGeometryNode cGeometryNode      cPagedLODNodeChild cPagedLODNodeChild cPagedLODNodeChild cPagedLODNodeChild
    //    cRenderable cRenderable         cGeometryNode cGeometryNode
    //cRenderable cRenderable

    class cCullVisitor
    {
    public:
      cCullVisitor(cSceneGraph& scenegraph, const render::cCamera& camera);

      void Visit(cSceneNode& node) { node.Cull(*this); }
      void Visit(cGroupNode& node) { node.Cull(*this); }
      void Visit(cSwitchNode& node) { node.Cull(*this); }
      void Visit(cLODNode& node) { node.Cull(*this); }
      void Visit(cModelNode& node) { node.Cull(*this); }
      void Visit(cPagedLODNode& node) { node.Cull(*this); }
      void Visit(cAnimationNode& node) { node.Cull(*this); }

      // TODO: We have to remove this, there has to be a better way of doing this?
      void Visit(character::cMd3* pModel, const spitfire::math::cMat4& matAbsolutePositionAndRotation);

      void Visit(cStateSet* pStateSet, const spitfire::math::cMat4& matAbsolutePositionAndRotation);
      void Visit(sky::cSkyDomeAtmosphereRenderer& node);

      const spitfire::math::cVec3& GetCameraPosition() const { return camera.GetEyePosition(); }

    private:
      cSceneGraph& scenegraph;
      const render::cCamera& camera;
    };


    // Render visitor does not visit cSceneGraph nodes, it visits the cRenderGraph nodes that have been collected
    class cRenderVisitor
    {
    public:
      cRenderVisitor(cSceneGraph& scenegraph, render::cContext& context, const math::cFrustum& frustum);

    private:
      void ApplyStateSet(cStateSet& stateSet);
      void UnApplyStateSet(cStateSet& stateSet);
    };




    // NOTE: One restriction on the scenegraph at the moment is that every camera must use the same skysystem.
    // It is impossible to have a video camera on another planet in the galaxy with one sky and then also view a planet with another sky,
    // but in most situations this is not incredibly limiting and if this behaviour is required, one solutions may be to build two separate scenegraphs.
    // TODO: Other versions of cSceneGraph such cSceneGraphOctree and cSceneGraphBSP could be created

    class cSceneGraph
    {
    public:
      friend class cUpdateVisitor;
      friend class cCullVisitor;
      friend class cRenderVisitor;

      cSceneGraph();
      ~cSceneGraph();

      void Create();
      void Destroy();

      cSceneNodeRef GetRoot() const { ASSERT(pRoot != nullptr); return pRoot; }
      cSkySystemRef GetSkySystem() const { ASSERT(pSkySystem != nullptr); return pSkySystem; }

      const math::cColour& GetBackgroundColour() const { return backgroundColour; }
      void SetBackgroundColour(const math::cColour& colour) { backgroundColour = colour; }

      const math::cColour& GetAmbientColour() const { return ambientColour; }
      void SetAmbientColour(const math::cColour& colour) { ambientColour = colour; }

      bool IsCullingEnabled() const { return bIsCullingEnabled; }
      void SetCulling(bool bEnable) { bIsCullingEnabled = bEnable; }

      void Update(sampletime_t currentTime);
      void Cull(sampletime_t currentTime, const render::cCamera& camera);
      void Render(sampletime_t currentTime, render::cContext& context, const math::cFrustum& frustum);


      // Cameras

      const std::vector<cCamera>& GetCameraList() const;


      // Nodes

      //cSceneNodeRef FindNode(const std::string& sName);

      // All create functions are here so that the scenegraph can enforce unique names (Among other things)
      cModelNodeRef CreateModelNode(const std::string& sName);
      cAnimationNodeRef CreateAnimationNode(const std::string& sName);
      cParticleSystemNodeRef CreateParticleSystemNode(const std::string& sName);
      cLightNodeRef CreateLightNode(const std::string& sName);

      void DestroyNode(cSceneNodeRef pNode);

      size_t GetNodeCount() const { return nNodes; }


      // Objects

      cEntityRef CreateEntity(const std::string& sName, const std::string& sMesh);
      cLightRef CreateLight(const std::string& sName);
      //cCameraRef CreateCamera(const std::string& sName);
      void DestroyLight(cLightRef pLight);
      void DestroyEntity(cEntityRef pEntity);
      //void DestroyCamera(cCameraRef pCamera);

      size_t GetEntityCount() const { return entites.size(); }
      size_t GetLightCount() const { return lights.size(); }
      //size_t GetCameraCount() const { return cameras.size(); }


    protected:
      cRenderGraph& GetRenderGraph() { return renderGraph; }

    private:
      //uint32_t UniqueNameToUniqueHash(const std::string& sName) const;

      bool bIsCullingEnabled;
      math::cColour backgroundColour;
      math::cColour ambientColour;

      cRenderGraph renderGraph;
      cSceneNodeRef pRoot;
      cSkySystemRef pSkySystem;

      size_t nNodes; // Count of the nodes that the scenegraph has created itself
      //std::map<uint32_t, cSceneNodeRef> nodes; // Unique hash to node map

      std::map<std::string, cEntityRef> entites;
      std::map<std::string, cLightRef> lights;
      //std::map<std::string, cCameraRef> cameras;
    };
  }
}

#endif // SCENEGRAPH_H

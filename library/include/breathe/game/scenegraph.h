#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include <breathe/breathe.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

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
  class cObject;

  namespace render
  {
    class cShader
    {
    public:
    };
  }


  namespace physics
  {
    class cPhysicsObject;
  }

  namespace vehicle
  {
    class cVehicle;
  }

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


    class cStateShader
    {
    public:
      cStateShader() : bHasValidValue(false), bTurnedOn(false) {}

      bool operator==(const cStateShader& rhs) const;
      bool operator!=(const cStateShader& rhs) const { return !(*this == rhs); }

      bool bHasValidValue;
      bool bTurnedOn;
      render::material::cShaderRef pShader;
    };

    inline bool cStateShader::operator==(const cStateShader& rhs) const
    {
      return (bHasValidValue == rhs.bHasValidValue) && (bTurnedOn == rhs.bTurnedOn) && (pShader == rhs.pShader);
    }
  }

  namespace scenegraph3d
  {
    class cSceneGraph;
    class cUpdateVisitor;
    class cCullVisitor;

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
      enum PRIORITY
      {
        PRIORITY_HIGH3 = 0,
        PRIORITY_HIGH2,
        PRIORITY_HIGH1,
        PRIORITY_HIGH0,
        PRIORITY_DIFFUSE,
        PRIORITY_TRANSPARENT,

        // Helper states
        PRIORITY_NORMAL = PRIORITY_DIFFUSE
      };

      cStateSet();
      cStateSet(const cStateSet& rhs);

      cStateSet& operator=(const cStateSet& rhs);

      bool operator==(const cStateSet& rhs) const;
      bool operator!=(const cStateSet& rhs) const { return !(*this == rhs); }

    private:
      PRIORITY priority;
      render::material::cMaterialRef material;

      scenegraph_common::cStateBoolean alphablending;
      scenegraph_common::cStateTexture texture;
      scenegraph_common::cStateShader shader;
    };

    inline cStateSet::cStateSet() :
      priority(PRIORITY_NORMAL)
    {
    }

    inline cStateSet::cStateSet(const cStateSet& rhs) :
      priority(rhs.priority),
      material(rhs.material),
      alphablending(rhs.alphablending),
      texture(rhs.texture),
      shader(rhs.shader)
    {
    }

    inline cStateSet& cStateSet::operator=(const cStateSet& rhs)
    {
      priority = rhs.priority;
      material = rhs.material;
      alphablending = rhs.alphablending;
      texture = rhs.texture;
      shader = rhs.shader;

      return *this;
    }

    inline bool cStateSet::operator==(const cStateSet& rhs) const
    {
      return (priority == rhs.priority) && (material == material) &&
        (alphablending == rhs.alphablending) && (texture == rhs.texture) && (shader == rhs.shader);
    }

    class cSceneNode;

    typedef cSmartPtr<cSceneNode> cSceneNodeRef;

    // Uses a quaternion to work out world rotation
    // Only call GenerateBoundingVolume at the very end of setting rotations and positions for all nodes

    class cSceneNode
    {
    public:
      friend class cUpdateVisitor;
      friend class cCullVisitor;

      virtual ~cSceneNode();

      const string_t& GetUniqueName() const { return sUniqueName; }
      void SetUniqueName(const string_t& _sUniqueName) { sUniqueName = _sUniqueName; }

#ifndef NDEBUG
      bool IsWithinNode(cSceneNodeRef pNode) const; // Determines whether this node is in the scene graph, ie. whether it's ultimate ancestor is the root scene node.

      // Checks that pChild is stored within our children
      // TODO: Implement this function
      bool IsParentOfChild(const cSceneNodeRef pChild) const { return false; }
#endif

      cSceneNodeRef GetParent() const { return pParent; }

      void AttachChild(cSceneNodeRef pChild); // Calls the virtual _AttachChild so that a scenegraph can choose how to add it or even not add it at all
      void DetachChildForUseLater(cSceneNodeRef pChild); // Doesn't touch children of pChild, just detaches from the scenegraph, so you can still reference pChild and reinsert it later
      void DeleteChildRecursively(cSceneNodeRef pChild); // Removes pChild from the scenegraph and also calls DeleteAllChildrenRecursively on each of the children of pChild
      void DeleteAllChildrenRecursively() { _DeleteAllChildrenRecursively(); }


      bool IsEnabled() const { return bIsEnabled; }
      void SetEnabled(bool _bIsEnabled) { bIsEnabled = _bIsEnabled; }

      bool IsDirty() const { return bIsDirty; }
      void SetDirty();

      void SetVisible(bool bVisible);
      void SetPosition(const math::cVec3& position);
      void SetRotation(const math::cQuaternion& rotation);

      const math::cVec3& GetLocalPosition() const { return relativePosition; }
      const math::cQuaternion& GetLocalRotation() const { return relativeRotation; }
      void SetLocalPosition(const math::cVec3& position);
      void SetLocalRotation(const math::cVec3& rotation);

      math::cVec3 GetGlobalPosition() const; // Calculated base on the parents of this node

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

    protected:
      // Only the derived classes can call use this constructor
      cSceneNode();

      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);

      float_t UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius();

      // Only the derived class will know how to get the radius from our possible children
      virtual float_t _UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius() { return math::cEPSILON; }

    private:
      cSceneNode(const cSceneNode&); // Prevent copying
      cSceneNode& operator=(const cSceneNode&); // Prevent copying

      void GenerateBoundingVolume();

      virtual void _AttachChild(cSceneNodeRef pChild) {}
      virtual void _DetachChild(cSceneNodeRef pChild) {}
      virtual void _DeleteChildRecursively(cSceneNodeRef pChild) {}
      virtual void _DeleteAllChildrenRecursively() {}


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
    };

    inline cSceneNode::~cSceneNode()
    {
      DeleteAllChildrenRecursively();
    }

    inline void cSceneNode::AttachChild(cSceneNodeRef pChild)
    {
      ASSERT(!IsParentOfChild(pChild));

      _AttachChild(pChild);

      pChild->pParent.reset(this);
    }

    inline void cSceneNode::DetachChildForUseLater(cSceneNodeRef pChild)
    {
      ASSERT(pChild != nullptr);
      ASSERT(IsParentOfChild(pChild));

      _DetachChild(pChild);

      pChild->pParent.reset();
    }

    inline void cSceneNode::DeleteChildRecursively(cSceneNodeRef pChild)
    {
      ASSERT(pChild != nullptr);
      ASSERT(IsParentOfChild(pChild));

      _DeleteChildRecursively(pChild);

      pChild->pParent.reset();
    }



    class cGroupNode : public cSceneNode
    {
    public:
      cGroupNode() {}

      void AddChild(cSceneNodeRef pChild);
      void RemoveChild(cSceneNodeRef pChild);

    protected:
      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);

    private:
      std::vector<cSceneNodeRef> children;
    };

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

    class cSwitchNode : public cSceneNode
    {
    public:
      cSwitchNode() : index(0) {}

      void SetIndex(size_t index);

    private:
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

    private:
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


    class cPagedLODNodeLoader
    {
    public:
      void Update() {}

    private:
      void LoadTerrainLOD0(size_t x, size_t y);
      void LoadTerrainLOD1(size_t x, size_t y);
      void LoadTerrainLOD2(size_t x, size_t y);

      void LoadGrass(size_t x, size_t y);
    };

    class cPagedLODNodeChild : public cSceneNode
    {
    public:

    private:
      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);

      cLODNode terrain; // Terrain heightmap
      //std::vector<cGeometryNodeRef> grass; // One cGeometryNode for each grass
      cLODNode trees; // Anything else gets added here such as trees
    };

    typedef cSmartPtr<cPagedLODNodeChild> cPagedLODNodeChildRef;

    class cPagedLODNode : public cSceneNode
    {
    public:

    private:
      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);

      cPagedLODNodeLoader loader;
      std::vector<cPagedLODNodeChildRef> node;

      // TODO: Could try a quadtree
      // cQuadTree<cPagedLODNodeChildRef> quadtree;
    };

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


    class cUpdateVisitor
    {
    public:
      explicit cUpdateVisitor(cSceneGraph& scenegraph);

      void Visit(cSceneNode& node) {}
      void Visit(cModelNode& node) {}
      void Visit(cLightNode& node) {}

      void Visit(sky::cSkyDomeAtmosphereRenderer& node);

    private:

    };




    //                                    cSceneNode
    //                  cLODNode                                cPagedLODNode
    //         cGeometryNode cGeometryNode      cPagedLODNodeChild cPagedLODNodeChild cPagedLODNodeChild cPagedLODNodeChild
    //    cRenderable cRenderable         cGeometryNode cGeometryNode
    //cRenderable cRenderable

    enum RENDER_PRIORITY
    {
      RENDER_PRIORITY_FIRST = -3,
      RENDER_PRIORITY_SECOND = -2,
      RENDER_PRIORITY_THIRD = -1,
      RENDER_PRIORITY_OPAQUE = 0,
      RENDER_PRIORITY_NORMAL = 0,
      RENDER_PRIORITY_TRANSPARENT,
      RENDER_PRIORITY_LAST
    };

    class cRenderable;
    typedef cSmartPtr<cRenderable> cRenderableRef;

    class cCullVisitor
    {
    public:
      explicit cCullVisitor(cSceneGraph& scenegraph);

      void Visit(cSceneNode& node) {}
      void Visit(cModelNode& node) {}
      void Visit(cLightNode& node)
      {
        AddLight(node);
      }

      void Visit(cRenderableRef pRenderable)
      {
        AddRenderable(pRenderable);
      }

      void Visit(sky::cSkyDomeAtmosphereRenderer& node);

    private:
      void AddRenderable(cRenderableRef pRenderable) {}
      void AddLight(cLightNode& light) {}

      cSceneGraph& scenegraph;
    };


    // Render visitor does not visit cSceneGraph nodes, it visits the cRenderGraph nodes that have been collected
    class cRenderVisitor
    {
    public:
      explicit cRenderVisitor(cSceneGraph& scenegraph);

    private:
      cSceneGraph& scenegraph;
    };






    typedef cSmartPtr<cSceneNode> cGroupNodeRef;


    class cRenderable
    {
    public:


    private:
      bool bUseIsShadowCasting;
      bool bIsShadowCasting;

      bool bUseRenderPriority;
      RENDER_PRIORITY renderPriority;
    };

    typedef cSmartPtr<cRenderable> cRenderableRef;


    class cRenderGraph
    {
    public:
      friend class cCullVisitor;
      friend class cRenderVisitor;

      void AddRenderable(const cRenderableRef renderable);
      void Clear();

    private:
      typedef std::list<cRenderable*> cRenderableList;

      // Opaque (states : list of renderables with that state)
      std::map<cStateSet*, cRenderableList*> mOpaque;

      // Transparent (distance from the camera : renderable at that position)
      std::map<float, cRenderableRef> mTransparent;
    };

    inline void cRenderGraph::Clear()
    {
      mOpaque.clear();
      mTransparent.clear();
    }




    // NOTE: A restriction on the scenegraph at the moment is that every camera must use the same skysystem.
    // It is impossible to have a video camera on another planet in the galaxy with one sky and then also view a planet with another sky,
    // but I don't think that is incredibly limiting and if that behaviour was required you would probably be better off building two scenegraphs anyway.

    class cSceneGraph
    {
    public:
      friend class cUpdateVisitor;
      friend class cCullVisitor;
      friend class cRenderVisitor;

      cSceneGraph();

      void Create();

      cGroupNodeRef GetRoot() const { ASSERT(pRoot != nullptr); return pRoot; }
      cSkySystemRef GetSkySystem() const { ASSERT(pSkySystem != nullptr); return pSkySystem; }

      bool IsCullingEnabled() const { return bIsCullingEnabled; }
      void SetCulling(bool bEnable) { bIsCullingEnabled = bEnable; }

      void Update(sampletime_t currentTime);
      void Cull(sampletime_t currentTime);
      void Render(sampletime_t currentTime);

    protected:
      cRenderGraph& GetRenderGraph() { return renderGraph; }

    private:
      bool bIsCullingEnabled;

      cRenderGraph renderGraph;
      cGroupNodeRef pRoot;
      cSkySystemRef pSkySystem;
    };
  }
}

#endif // SCENEGRAPH_H

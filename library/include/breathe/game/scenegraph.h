#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include <breathe/breathe.h>
#include <breathe/math/math.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cQuaternion.h>

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

  namespace scenegraph
  {
    class cSceneGraph;

    class cRenderState
    {
    public:
      cRenderState(const cRenderState& rhs);

      cRenderState& operator=(const cRenderState& rhs);

      bool operator==(const cRenderState& rhs);
      bool operator!=(const cRenderState& rhs) { return !(*this == rhs); }
    };

    inline bool cRenderState::operator==(const cRenderState& rhs)
    {
      // To start with all states are equal
      return true;
    }




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

    class cUpdateVisitor;
    class cCullVisitor;

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

    private:
      PRIORITY priority;
    };

    class cSceneNode;

    typedef cSmartPtr<cSceneNode> cSceneNodeRef;

    // Uses a quaternion to work out world rotation
    // Only call GenerateBoundingVolume at the very end of setting rotations and positions for all nodes

    class cSceneNode
    {
    public:
      friend class cUpdateVisitor;
      friend class cCullVisitor;

      virtual ~cSceneNode() {}

      bool IsWithinNode(cSceneNodeRef pNode) const; // Determines whether this node is in the scene graph, ie. whether it's ultimate ancestor is the root scene node.

      cSceneNodeRef GetParent() const { return pParent; }

      void AttachChild(cSceneNode* pChild) { _AttachChild(pChild); }
      void DetachFromParent() { pParent.reset(); }


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
      const bool IsShowingBoundingBox() const;
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

      // This should be pure virtual?
      virtual void _AttachChild(cSceneNode* pChild) {}

      // Only called by a scenenode to another scenenode
      void AttachToParent(cSceneNodeRef _pParent);


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

#ifdef BUILD_DEBUG
      bool bIsShowingBoundingBox;
#endif

      math::cSphere boundingSphere;
      math::cBox boundingBox;

      cSceneNodeRef pParent; // Each node has exactly one parent, no more no less.  pRoot is the only exception, pRoot->pParent == nullptr;
    };

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
      cSpatialGraphNode() { memset(&pChild[0], 0, sizeof(cSpatialGraphNode*) * 8); }
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

      void Visit(cSceneNode& node) { printf("cUpdateVisitor::Visit cSceneNode\n"); }
      void Visit(cModelNode& node) { printf("cUpdateVisitor::Visit cModelNode\n"); }
      void Visit(cLightNode& node) { printf("cUpdateVisitor::Visit cLightNode\n"); }

    private:

    };



    // http://lightfeather.de/news.php

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

      void Visit(cSceneNode& node) { printf("cCullVisitor::Visit cSceneNode\n"); }
      void Visit(cModelNode& node) { printf("cCullVisitor::Visit cModelNode\n"); }
      void Visit(cLightNode& node)
      {
        AddLight(node);
      }

      void Visit(cRenderableRef pRenderable)
      {
        AddRenderable(pRenderable);
      }

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






    class cScopedEnable
    {
    public:
      explicit cScopedEnable(GLenum field);
      ~cScopedEnable();

    private:
      GLenum field;
    };

    inline cScopedEnable::cScopedEnable(GLenum _field) :
      field(_field)
    {
      glEnable(field);
    }

    inline cScopedEnable::~cScopedEnable()
    {
      glDisable(field);
    }

    class cScopedAttributes
    {
    public:
      explicit cScopedAttributes(GLbitfield attribute);
      ~cScopedAttributes();

    private:
      GLbitfield attributes;
    };

    inline cScopedAttributes::cScopedAttributes(GLbitfield _attribute) :
      attributes(_attribute)
    {
      glPushAttrib(attributes);
    }

    inline cScopedAttributes::~cScopedAttributes()
    {
      glPopAttrib();
    }




    // State classes for keeping track of what state we are in.
    // Default compiler generated copy constructors are fine for these simple classes.

    class cStateBoolean
    {
    public:
      cStateBoolean() : bHasValidValue(false), bTurnedOn(false) {}

      bool bHasValidValue;
      bool bTurnedOn;
    };

    class cStateBooleanWithFloat
    {
    public:
      cStateBooleanWithFloat() : bHasValidValue(false), bTurnedOn(false), fValue(0.0f) {}

      bool bHasValidValue;
      bool bTurnedOn;
      float fValue;
    };

    class cStateTexture
    {
    public:
      cStateTexture() : bHasValidValue(false), bTurnedOn(false) {}

      bool bHasValidValue;
      bool bTurnedOn;
      render::cTextureRef pTexture;
    };

    class cStateShader
    {
    public:
      cStateShader() : bHasValidValue(false), bTurnedOn(false) {}

      bool bHasValidValue;
      bool bTurnedOn;
      render::material::cShaderRef pShader;
    };

    class cStateList
    {
    private:
      cStateBoolean alphablending;
      cStateTexture texture;
      cStateShader shader;
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
      std::map<cRenderState*, cRenderableList*> mOpaque;

      // Transparent (distance from the camera : renderable at that position)
      std::map<float, cRenderableRef> mTransparent;
    };

    inline void cRenderGraph::Clear()
    {
      mOpaque.clear();
      mTransparent.clear();
    }

    class cSceneGraph
    {
    public:
      friend class cUpdateVisitor;
      friend class cCullVisitor;
      friend class cRenderVisitor;

      cSceneGraph();

      cGroupNodeRef GetRoot() const { ASSERT(pRoot != nullptr); return pRoot; }

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
    };
  }
}

#endif // SCENEGRAPH_H
